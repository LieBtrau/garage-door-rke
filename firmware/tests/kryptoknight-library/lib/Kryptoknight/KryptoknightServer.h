#pragma once

#include "Kryptoknight.h"

class KryptoknightServer : public Kryptoknight
{
public:
    typedef byte* (*getKey_Function)(uint32_t clientId);
    KryptoknightServer();
    ~KryptoknightServer();
    bool handleIncomingPacket(byte *packet, byte packet_length);
    void setGetKeyEvent(getKey_Function getKey);
private:
    enum
    {
        WAITING_FOR_CLIENT_HELLO,
        WAITING_FOR_MAC_BA,
    } _state = WAITING_FOR_CLIENT_HELLO;
    getKey_Function _getKey = nullptr;
    byte* _ssKey;
};

