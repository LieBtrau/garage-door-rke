#pragma once

#include "Kryptoknight.h"

class KryptoknightClient : public Kryptoknight
{
private:
    enum
    {
        IDLE,
        WAITING_FOR_NONCE_A,
        WAITING_FOR_MAC_AB
    } _state = IDLE;
    byte *_ssk;

public:
    KryptoknightClient(uint32_t my_id, byte *shared_secret_key, uint32_t server_id);
    ~KryptoknightClient();
    void startAuthentication();
    bool handleIncomingPacket(byte *packet, byte packet_length);
};
