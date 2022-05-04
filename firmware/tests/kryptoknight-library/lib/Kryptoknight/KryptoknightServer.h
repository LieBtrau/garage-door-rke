#pragma once

#include "Kryptoknight.h"

class KryptoknightServer : public Kryptoknight
{
private:
    enum
    {
        WAITING_FOR_CLIENT_HELLO,
        WAITING_FOR_MAC_BA,
    } _state = WAITING_FOR_CLIENT_HELLO;
public:
    KryptoknightServer(byte* shared_secret_key);
    ~KryptoknightServer();
    bool rx_handler(byte *packet, byte packet_length);
};

