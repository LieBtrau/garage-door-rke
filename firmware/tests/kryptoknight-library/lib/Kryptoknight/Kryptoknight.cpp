#include "Kryptoknight.h"

Kryptoknight::Kryptoknight()
{
    protocol_timeout.start(PROTOCOL_TIMEOUT, AsyncDelay::MILLIS);
}

Kryptoknight::~Kryptoknight()
{
}

void Kryptoknight::setTransmitPacketEvent(TX_Function txfunc)
{
    _txfunc = txfunc;
}

void Kryptoknight::setMutualAuthentication(bool isEnabled)
{
    _mutualAuthentication = isEnabled;
}

/**
 * @brief message_ba which is the concatentation of nonce_A, nonce_B and client_id
 *
 */
void Kryptoknight::generate_message_ba()
{
    if (_mutualAuthentication)
    {
        memcpy(_message_ba, _nonce_A, sizeof(_nonce_A));
        memcpy(_message_ba + sizeof(_nonce_A), _nonce_B, sizeof(_nonce_B));
        memcpy(_message_ba + sizeof(_nonce_A) + sizeof(_nonce_B), &_client_id, sizeof(_client_id));
    }
    else
    {
        memcpy(_message_ba, _nonce_A, sizeof(_nonce_A));
        memcpy(_message_ba + sizeof(_nonce_A), &_client_id, sizeof(_client_id));
    }
}

void Kryptoknight::generate_message_ab()
{
    memcpy(_message_ab, _nonce_A, sizeof(_nonce_A));
    memcpy(_message_ab + sizeof(_nonce_A), _nonce_B, sizeof(_nonce_B));
}