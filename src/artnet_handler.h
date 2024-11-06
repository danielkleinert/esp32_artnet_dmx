#pragma once

#include <ArtnetWifi.h>
#include <functional>
#include "fps_counter.h"

class ArtnetHandler {
public:
    using DmxCallback = std::function<void(uint16_t channel, uint8_t value)>;
    
    static void init(DmxCallback callback, uint16_t universe = 1);
    static void read();
    static float getFPS();
    static void setPaused(bool paused);

private:
    static void handlePacket(uint16_t universe, uint16_t length, uint8_t sequence, uint8_t* data);
    
    static ArtnetWifi artnet;
    static uint16_t universe;
    static DmxCallback dmxCallback;
    static bool paused;
    static FPSCounter fpsCounter;
};