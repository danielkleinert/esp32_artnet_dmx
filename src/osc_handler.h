#pragma once

#include <Arduino.h>
#include <ArduinoOSCWiFi.h>

class OscHandler {
private:
    static void (*dmxCallback)(uint16_t channel, uint8_t value);
    static unsigned long lastMessageTime;
    static bool isPaused; 

public:
    static void init(void (*callback)(uint16_t channel, uint8_t value), uint16_t port);
    static void read();
    static void setPaused(bool paused);
    static unsigned long getTimeSinceLastMessage();
};