#pragma once
#include "Arduino.h"
#include "AsyncDelay.h"
#include "sodium.h"

/**
 * @brief 2PAP Authentication protocol
 * Client                                                                       Server
 * -------------------                                                          ------------------------ 
 *                          -> client_id ->                                     WAITING_FOR_CLIENT_HELLO
 * WAITING_FOR_NONCE_A      <- nonce_A   <-    
 *                          -> Nonce_B, MACba(Nonce_A, Nonce_B, client_id) ->   WAITING_FOR_MAC_BA
 * WAITING_FOR_MAC_AB       <- MACab(Nonce_A, Nonce_B) <-
 * 
 * 
 * Remarks:
 * --------
 * 1. If the client doesn't require the server to authenticate, then some air time can be saved.  
 *      Nonce_B is not needed in the third message.
 *      The forth message can be left out.
 * 2. MACab and MACba don't necessarily need to use the same key.  
 */

class Kryptoknight
{
public:
    typedef bool (*TX_Function)(byte *data, byte length);
    Kryptoknight();
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
    byte _nonce_A[crypto_secretbox_NONCEBYTES] = {0};
    byte _nonce_B[crypto_secretbox_NONCEBYTES] = {0};
    byte _mac_ba[crypto_auth_BYTES] = {0};
    byte _mac_ab[crypto_auth_BYTES] = {0};
    byte _message_ba[sizeof(_nonce_A) + sizeof(_nonce_B) + sizeof(_client_id)] = {0};
    byte _message_ab[sizeof(_nonce_A) + sizeof(_nonce_B)] = {0};
    bool _mutualAuthentication = true;

private:
};
