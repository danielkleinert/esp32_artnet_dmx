#include "osc_handler.h"

void (*OscHandler::dmxCallback)(uint16_t channel, uint8_t value) = nullptr;
unsigned long OscHandler::lastMessageTime = 0;
bool OscHandler::isPaused = false;

void OscHandler::init(void (*callback)(uint16_t channel, uint8_t value), uint16_t port) {
    dmxCallback = callback;
    lastMessageTime = 0; 

    OscWiFi.subscribe(port, "/dmx/*",
        [](const OscMessage& m) {
            lastMessageTime = millis();
            if (isPaused) return;

            String addr = m.address();
            String channelStr = addr.substring(5);  // Remove "/dmx/"
            // Verify the remaining string contains only digits
            for (size_t i = 0; i < channelStr.length(); i++) {
                if (!isDigit(channelStr[i])) {
                    return;  // Invalid channel format
                }
            }

            int channel = channelStr.toInt() - 1;
            if (channel < 0 || channel >= 512 || m.size() == 0) return;

            if (m.isInt32(0)) {  // Integer value (0-255)
                int value = m.arg<int>(0);
                if (value < 0 || value > 255) return;
                OscHandler::dmxCallback(channel, value);
                return;
            }
            
            if (m.isFloat(0)) {  // Float value (0.0-1.0)
                float value = m.arg<float>(0);
                if (value < 0.0f || value > 1.0f) return;
                uint8_t dmxValue = static_cast<uint8_t>(value * 255);
                OscHandler::dmxCallback(channel, dmxValue);
                return;
            }
        });
}

void OscHandler::read() {
    OscWiFi.parse();
}

void OscHandler::setPaused(bool paused) {
    isPaused = paused;
}

unsigned long OscHandler::getTimeSinceLastMessage() {
    return lastMessageTime > 0 ? millis() - lastMessageTime : 0;
}