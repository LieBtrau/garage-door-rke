#include "KryptoknightClient.h"

#define SP Serial
#define DEBUG

KryptoknightClient::KryptoknightClient(uint32_t my_id, byte *shared_secret_key, uint32_t server_id) : _ssk(shared_secret_key)
{
    _client_id = my_id;
    _server_id = server_id;
}

KryptoknightClient::~KryptoknightClient()
{
}

void KryptoknightClient::startAuthentication()
{
    protocol_timeout.restart();
    byte id[sizeof(_client_id)];
    memcpy(id, &_client_id, sizeof(_client_id));
    _state = WAITING_FOR_NONCE_A;
    if (_txfunc != nullptr)
    {
        _txfunc(id, sizeof(id));
    }
}

/**
 * @brief Handle incoming 2PAP-protocol messages
 *
 * @param packet
 * @param packet_length
 * @return true when authentication is successful
 * @return false : authentication busy or failed
 */
bool KryptoknightClient::handleIncomingPacket(byte *packet, byte packet_length)
{
    if (protocol_timeout.isExpired())
    {
#ifdef DEBUG
        SP.println("Client Timeout");
#endif
        _state = IDLE;
    }
    switch (_state)
    {
    case WAITING_FOR_NONCE_A:
        if (packet_length != crypto_secretbox_NONCEBYTES)
        {
            _state = IDLE;
#ifdef DEBUG
            SP.println("Nonce A format wrong");
#endif
            return false;
        }
        // Store NONCE_A
        memcpy(_nonce_A, packet, packet_length);
        // Prepare the following message in the protocol
        if (_mutualAuthentication)
        {
            // Create NONCE_B
            randombytes_buf(_nonce_B, crypto_secretbox_NONCEBYTES);
            // Client calculates mac_ba from message_ba
            generate_message_ba();
            crypto_auth(_mac_ba, _message_ba, sizeof(_message_ba), _ssk);
            // Client sends the following to the server : Nonce_B, MACba
            byte message2[sizeof(_nonce_B) + sizeof(_mac_ba)];
            memcpy(message2, _nonce_B, sizeof(_nonce_B));
            memcpy(message2 + sizeof(_nonce_B), _mac_ba, sizeof(_mac_ba));
            _state = WAITING_FOR_MAC_AB;
            if (_txfunc == nullptr || !_txfunc(message2, sizeof(message2)))
            {
#ifdef DEBUG
                SP.println("Server doesn't ack message2");
#endif
                _state = IDLE;
                return false;
            }
        }
        else
        {
            // NONCE_B not needed
            generate_message_ba();
            // Sign MAC
            crypto_auth(_mac_ba, _message_ba, sizeof(_message_ba) - sizeof(_nonce_B), _ssk);
            // Client sends the following to the server : MACba(Nonce_A, id_B)
            _state = IDLE;
            if (_txfunc == nullptr || !_txfunc(_mac_ba, sizeof(_mac_ba)))
            {
#ifdef DEBUG
                SP.println("Server doesn't ack message2");
#endif
                _state = IDLE;
                return false;
            }
        }
        return false;
    case WAITING_FOR_MAC_AB:
        _state = IDLE;
        if (packet_length != crypto_auth_BYTES)
        {
#ifdef DEBUG
            SP.println("MAC_AB format wrong");
#endif
            return false;
        }
        // Store mac_ab
        memcpy(_mac_ab, packet, packet_length);
        generate_message_ab();
        return crypto_auth_verify(_mac_ab, _message_ab, sizeof(_message_ab), _ssk) == 0;
    case IDLE:
    default:
        return false;
    }
}