#include "artnet_handler.h"
#include <Arduino.h>

ArtnetWifi ArtnetHandler::artnet;
uint16_t ArtnetHandler::universe = 1;
ArtnetHandler::DmxCallback ArtnetHandler::dmxCallback;
FPSCounter ArtnetHandler::fpsCounter;
bool ArtnetHandler::paused = false;

void ArtnetHandler::init(DmxCallback callback, uint16_t universeNum) {
    universe = universeNum;
    dmxCallback = callback;
    artnet.begin();
    artnet.setArtDmxCallback(ArtnetHandler::handlePacket);
}

void ArtnetHandler::read() {
    artnet.read();
}

void ArtnetHandler::handlePacket(uint16_t universe, uint16_t length, uint8_t sequence, uint8_t* data) {
    fpsCounter.count();

    if (!paused && universe == ArtnetHandler::universe) {
        for (int i = 0; i < length; i++) {
            dmxCallback(i, data[i]);
        }
    }
}

void ArtnetHandler::setPaused(bool p) {
    paused = p;
}

float ArtnetHandler::getFPS() { 
    return fpsCounter.getFPS(); 
}