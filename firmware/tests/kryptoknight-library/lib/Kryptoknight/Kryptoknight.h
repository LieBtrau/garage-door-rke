#pragma once
#include "Arduino.h"
#include "AsyncDelay.h"
#include "sodium.h"

class Kryptoknight
{
public:
    typedef bool (*TX_Function)(byte *data, byte length);
    Kryptoknight(byte *shared_secret_key);
    ~Kryptoknight();
    void setTransmitPacketEvent(TX_Function txfunc);
    virtual bool handleIncomingPacket(byte *packet, byte packet_length) = 0;
    void setMutualAuthentication(bool isEnabled);

protected:
    void generate_message_ba();
    void generate_message_ab();
    const int PROTOCOL_TIMEOUT = 750;
    AsyncDelay protocol_timeout;
    TX_Function _txfunc = nullptr;
    uint32_t _client_id;
    byte *_ssk;
    byte _nonce_A[crypto_secretbox_NONCEBYTES] = {0};
    byte _nonce_B[crypto_secretbox_NONCEBYTES] = {0};
    byte _mac_ba[crypto_auth_BYTES] = {0};
    byte _mac_ab[crypto_auth_BYTES] = {0};
    byte _message_ba[sizeof(_nonce_A) + sizeof(_nonce_B) + sizeof(_client_id)] = {0};
    byte _message_ab[sizeof(_nonce_A) + sizeof(_nonce_B)] = {0};
    bool _mutualAuthentication = true;

private:
};
