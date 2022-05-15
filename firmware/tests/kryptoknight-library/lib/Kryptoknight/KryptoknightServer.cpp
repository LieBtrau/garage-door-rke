#include "KryptoknightServer.h"

KryptoknightServer::KryptoknightServer(byte *shared_secret_key) : Kryptoknight(shared_secret_key)
{
}

KryptoknightServer::~KryptoknightServer()
{
}

/**
 * @brief Handle incoming 2PAP-protocol messages
 *
 * @param packet
 * @param packet_length
 * @return true when authentication is successful
 * @return false : authentication busy or failed
 */
bool KryptoknightServer::handleIncomingPacket(byte *packet, byte packet_length)
{
    if (protocol_timeout.isExpired())
    {
        Serial.println("Server Timeout");
        _state = WAITING_FOR_CLIENT_HELLO;
    }
    switch (_state)
    {
    case WAITING_FOR_CLIENT_HELLO:
        Serial.println("WAITING_FOR_CLIENT_HELLO");
        if (packet_length != sizeof(_client_id))
        {
            return false;
        }
        protocol_timeout.restart();
        // Store B (=client_id)
        memcpy(&_client_id, packet, packet_length);
        // Generate nonce_A
        randombytes_buf(_nonce_A, crypto_secretbox_NONCEBYTES);

        //Debug
        memset(_nonce_A, 0xAA, crypto_secretbox_NONCEBYTES);

        // Send nonce_A
        _state = WAITING_FOR_MAC_BA;
        if (_txfunc != nullptr && !_txfunc(_nonce_A, sizeof(_nonce_A)))
        {
            Serial.println("TX-error");
            _state = WAITING_FOR_CLIENT_HELLO;
            return false;
        }
        break;
    case WAITING_FOR_MAC_BA:
        Serial.println("WAITING_FOR_MAC_BA");
        if (packet_length != sizeof(_nonce_B) + sizeof(_mac_ba))
        {
            Serial.println("MAC_BA format wrong");
            _state = WAITING_FOR_CLIENT_HELLO;
            return false;
        }
        // Store nonce_B
        memcpy(_nonce_B, packet, sizeof(_nonce_B));
        // Store mac_ba
        memcpy(_mac_ba, packet + sizeof(_nonce_B), sizeof(_mac_ba));
        // Check if mac_ba is valid
        generate_message_ba();
        if (crypto_auth_verify(_mac_ba, _message_ba, sizeof(_message_ba), _ssk) != 0)
        {
            // Message was forged.
            _state = WAITING_FOR_CLIENT_HELLO;
            Serial.println("Server detects forged message.");
            return false;
        }
        // At this point, the server has authenticated the client.
        // For mutual authentication (i.e. the client knows for sure that its talking to the correct server, an extra message is needed.)
        _state = WAITING_FOR_CLIENT_HELLO;
        if (_mutualAuthentication)
        {
            //  Prepare mac_ab
            generate_message_ab();
            crypto_auth(_mac_ab, _message_ab, sizeof(_message_ab), _ssk);
            return (_txfunc != nullptr && _txfunc(_mac_ab, sizeof(_mac_ab)));
        }
        else
        {
            return true;
        }
    default:
        break;
    }
    return false;
}