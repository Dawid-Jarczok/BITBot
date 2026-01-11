#include "Pointer.h"

void Pointer::begin() {
}

void Pointer::iterate() {
    static uint32_t lastUpdate = 0;
    if (millis() - lastUpdate < _updateInterval) return;
    lastUpdate = millis();

    float intervalSec = (float)_updateInterval / 1000.0f;
    _xVel += _joystick->readX() * _acceleration * intervalSec;
    _yVel += _joystick->readY() * _acceleration * intervalSec;
    _xVel = constrain(_xVel, -_maxVelocity, _maxVelocity);
    _yVel = constrain(_yVel, -_maxVelocity, _maxVelocity);

    _x += _xVel * intervalSec;
    _y += _yVel * intervalSec;

    if (_x < 0.0f) {
        _x = 0.0f;
        _xVel = 0.0f;
    } else if (_x >= (float)_maxX) {
        _x = (float)_maxX-0.01f;
        _xVel = 0.0f;
    }
    if (_y < 0.0f) {
        _y = 0.0f;
        _yVel = 0.0f;
    } else if (_y >= (float)_maxY) {
        _y = (float)_maxY-0.01f;
        _yVel = 0.0f;
    }
}