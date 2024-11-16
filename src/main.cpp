#include <Arduino.h>
#include <WiFiManager.h>
#include <ESPmDNS.h>

#include "i2s_dmx.h"
#include "artnet_handler.h"
#include "web_ui.h"

#define DMX_CHANNELS 512
#define VERSION __DATE__ " / " __TIME__

const uint8_t I2S_PIN = 13;
const char* DEVICE_NAME = "IllumiNaughty";
const uint16_t ARTNET_UNIVERSE = 1;

uint8_t dmxValues[DMX_CHANNELS] = {0};

WiFiManager wifiManager;
I2SDMX dmx(I2S_PIN);

void onDmxValue(uint16_t channel, uint8_t value) {
    dmxValues[channel] = value;
    WebUI::updateChannelValue(channel, value);
}


void setup() {
  Serial.begin(115200);

  WiFiManager wifiManager;
  wifiManager.autoConnect(DEVICE_NAME);

  if (MDNS.begin(DEVICE_NAME)) {
    MDNS.addService("http", "tcp", 80);
    Serial.printf("Access at http://%s.local\n", DEVICE_NAME);
  } 

  dmx.begin();
  ArtnetHandler::init(onDmxValue, ARTNET_UNIVERSE);
  
  WebUI::init(String(DEVICE_NAME));
  WebUI::onSliderChange(dmxValues);
}


void loop() {
  ArtnetHandler::read();
  dmx.update(dmxValues);

  static unsigned long lastStatusUpdate = 0;
  if (millis() - lastStatusUpdate > 500) {
      String status = "Art-Net FPS: " + String(ArtnetHandler::getFPS(), 1) + 
                      "\nDMX FPS: " + String(dmx.getFPS(), 1);
      
      Serial.println(status);
      WebUI::updateStatus(status);
      lastStatusUpdate = millis();
  }
} 