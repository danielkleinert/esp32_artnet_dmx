#pragma once

#include <ESPUI.h>

class WebUI {
public:
    static void init(const String& deviceName);
    static void updateChannelValue(int channel, uint8_t value);
    static void updateStatus(const String& status);
    static void onSliderChange(uint8_t* dmxValues);

private:
    static String title;
    static void sliderCallback(Control* sender, int type);
    static void pauseArtnetCallback(Control* sender, int value);
    static uint16_t sliderIds[];
    static uint16_t statusId;
    static uint8_t* dmxValuesPtr;
};