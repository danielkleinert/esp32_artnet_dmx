#include "i2s_dmx.h"

I2SDMX::I2SDMX(int pin, unsigned int delay) : _pin(pin), _delay(delay) {
    _tic_loop = millis();
}

void I2SDMX::begin() {
    pinMode(_pin, OUTPUT);
    digitalWrite(_pin, 1);

    memset(&_i2sData, 0x00, sizeof(_i2sData));
    memset(&_i2sData.mark_before_break, 0xff, sizeof(_i2sData.mark_before_break));
    
    // 3 bits (12us) MAB. The MAB's LSB 0 acts as the start bit (low) for the null byte
    _i2sData.mark_after_break = (uint16_t) 0b000001110;

    // Initialize DMX bytes with stop/start bits
    for (int i = 0; i < DMX_CHANNELS; i++) {
        _i2sData.dmx_bytes[i] = (uint16_t) 0b0000000011111110;
    }
    _i2sData.dmx_bytes[DMX_CHANNELS] = (uint16_t) 0b0000000011111111;

    // Configure I2S
    i2s_config_t i2s_config = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
        .sample_rate = 7812,  // 250,000 baud / 32 bits
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
        .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
        .communication_format = I2S_COMM_FORMAT_STAND_I2S,
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
        .dma_buf_count = 8,
        .dma_buf_len = 64,
        .use_apll = false
    };

    i2s_pin_config_t pin_config = {
        .bck_io_num = -1,
        .ws_io_num = -1,
        .data_out_num = _pin,
        .data_in_num = -1
    };

    i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
    i2s_set_pin(I2S_NUM_0, &pin_config);
}

void I2SDMX::update(uint8_t* dmxValues) {
    unsigned long now = millis();
    if ((now - _tic_loop) > _delay) {
        _tic_loop = now;
        write(dmxValues);
    }
}

void I2SDMX::write(uint8_t* dmxValues) {
    _fpsCounter.count();
    for (int i = 0; i < DMX_CHANNELS; i++) {
        uint16_t hi = flipByte(dmxValues[i]);
        uint16_t lo = i == DMX_CHANNELS - 1 ? 0b0000000011111111 : 0b0000000011111110;
        _i2sData.dmx_bytes[i + 1] = (hi << 8) | lo;
    }

    size_t bytes_written;
    i2s_write(I2S_NUM_0, &_i2sData, sizeof(_i2sData), &bytes_written, portMAX_DELAY);
}


float I2SDMX::getFPS() { 
    return _fpsCounter.getFPS(); 
}

// Reverse byte order because DMX expects LSB first but I2S sends MSB first.
byte I2SDMX::flipByte(byte c) {
    c = ((c >> 1) & 0b01010101) | ((c << 1) & 0b10101010);
    c = ((c >> 2) & 0b00110011) | ((c << 2) & 0b11001100);
    return (c >> 4) | (c << 4);
}