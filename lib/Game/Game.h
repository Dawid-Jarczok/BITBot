#pragma once

#include <Arduino.h>

class Game {
public:
    Game() {};

    void begin();
    void iterate();
    void updatePositions(float pointerX, float pointerY, float targetX, float targetY);
    void start();
    void stop();

    void setPointerRadius(float radius) { _pointerX = radius; }
    void setUpdateInterval(uint16_t intervalMs) { _updateInterval = intervalMs; }

    float getScore() { return _score; }
    uint32_t getGameTime() { return _gameTime; }
    bool isTargetInPointer() { return _isTargetInPointer; }
    bool isRunning() { return _isRunning; }

private:
    bool _isRunning = false;
    float _score = 0.0f;
    uint32_t _gameStartTime = 0;
    uint32_t _gameTime = 0;
    uint32_t _gameDuration = 30000;

    uint16_t _updateInterval = 10;

    float _targetX = 0.0f;
    float _targetY = 0.0f;
    float _pointerX = 0.0f;
    float _pointerY = 0.0f;
    float _pointerRadius = 0.5f;
    bool _isTargetInPointer = false;

};