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

public:
    KryptoknightClient(uint32_t my_id, byte *shared_secret_key);
    ~KryptoknightClient();
    void startAuthentication();
    bool handleIncomingPacket(byte *packet, byte packet_length);
};
