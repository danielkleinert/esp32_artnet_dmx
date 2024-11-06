#pragma once

class FPSCounter {
public:
    FPSCounter() : _frameCounter(0), _tic_fps(millis()), _fps(0) {}

    void count() {
        _frameCounter++;
    }

    float getFPS() {
        unsigned long now = millis();
        unsigned long elapsed = now - _tic_fps;
        
        if (elapsed > 0) {
            _fps = (1000.0 * _frameCounter) / elapsed;
            _frameCounter = 0;
            _tic_fps = now;
        }
        return _fps;
    }

private:
    unsigned long _frameCounter;
    unsigned long _tic_fps;
    float _fps;
};