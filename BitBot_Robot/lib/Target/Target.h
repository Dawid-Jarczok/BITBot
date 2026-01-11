#pragma once
#include <Arduino.h>

class Target {
public:
    Target(uint16_t maxX, uint16_t maxY)
        : _maxX(maxX), _maxY(maxY){
    };

    void begin();

    void reset();
    void start() { isRunning = true; }
    void stop() { isRunning = false; }

    inline void setMaxVelocity(float maxV) { _maxVelocity = maxV; }
    inline void setAcceleration(float acc) { _acceleration = acc; }
    inline void setUpdateInterval(uint16_t intervalMs) { _updateInterfal = intervalMs; }
    
    inline float getX() { return _x; }
    inline float getY() { return _y; }
    inline float getX_target() { return _targetX; }
    inline float getY_target() { return _targetY; }
    inline uint16_t getXInt() { return (uint16_t)_x; }
    inline uint16_t getYInt() { return (uint16_t)_y; }
    inline float getXVelocity() { return _xVel; }
    inline float getYVelocity() { return _yVel; }
    inline float getMaxVelocity() { return _maxVelocity; }
    inline float getAcceleration() { return _acceleration; }
    void iterate();

private:
    const uint16_t _maxX;
    const uint16_t _maxY;

    uint16_t _updateInterfal = 10;
    bool isRunning = false;

    float _maxVelocity = 1.0f;
    float _acceleration = 0.5f;

    float _x = 0.0f;
    float _y = 0.0f;

    float _targetX = 0.0f;
    float _targetY = 0.0f;

    float _xVel = 0.0f;
    float _yVel = 0.0f;

    void spawningPoint(uint8_t &X, uint8_t &Y);
    void spawnNewTarget();

    float sign(float distance);
};