#pragma once

#include <Arduino.h>
#include <driver/I2S.h>
#include "fps_counter.h"

class I2SDMX {
public:
    static const int DMX_CHANNELS = 512;
    
    I2SDMX(int pin, unsigned int delay = 25);
    void begin();
    void update(uint8_t* dmxValues);
    float getFPS();

private:
    int _pin;
    unsigned int _delay;
    unsigned long _tic_loop = 0;
    FPSCounter _fpsCounter;
    
    // Packet structure for DMX over I2S
    typedef struct {
        uint16_t mark_before_break[1];          // 1 * 16 bits * 4 us -> 64 us
        uint16_t space_for_break[1];            // 1 * 16 bits * 4 us -> 64 us
        uint16_t mark_after_break;              // 13 MSB low bits * 4 us adds 52 us
        uint16_t dmx_bytes[DMX_CHANNELS + 1];   // each "byte" (actually a word) consists of: 8 bits payload + 7 stop bits (high) + 1 start (low) for the next byte
    } i2s_packet;

    i2s_packet _i2sData;
    
    void write(uint8_t* dmxValues);
    static byte flipByte(byte c);
};