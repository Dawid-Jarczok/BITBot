#pragma once
#include <Arduino.h>
#include "Joystick.h"

class Pointer {
public:
    Pointer(Joystick* joystick, uint16_t maxX, uint16_t maxY)
        : _maxX(maxX), _maxY(maxY), _joystick(joystick) {
    };
    void begin();
    inline void setMaxVelocity(float maxV) { _maxVelocity = maxV; }
    inline void setAcceleration(float acc) { _acceleration = acc; }
    inline void setUpdateInterval(uint16_t intervalMs) { _updateInterfal = intervalMs; }
    
    inline float getX() { return _x; }
    inline float getY() { return _y; }
    inline uint16_t getXInt() { return (uint16_t)_x; }
    inline uint16_t getYInt() { return (uint16_t)_y; }
    inline float getXVelocity() { return _xVel; }
    inline float getYVelocity() { return _yVel; }
    void iterate();

private:
    const uint16_t _maxX;
    const uint16_t _maxY;

    uint16_t _updateInterfal = 10;

    float _maxVelocity = 1.0f;
    float _acceleration = 0.5f;

    float _x = 0.0f;
    float _y = 0.0f;
    float _xVel = 0.0f;
    float _yVel = 0.0f;

    Joystick *_joystick = nullptr;
};