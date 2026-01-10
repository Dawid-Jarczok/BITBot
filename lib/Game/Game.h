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
    void setGameDuration(uint32_t durationMs) { _gameDuration = durationMs; }

    inline float getScore() { return _score; }
    inline float getMaxScore() { return _maxScore; }
    inline uint32_t getGameTime() { return _gameTime; }
    inline bool isTargetInPointer() { return _isTargetInPointer; }
    bool isRunning() { return _isRunning; }
    uint32_t getGameTimeLeft();

private:
    bool _isRunning = false;
    float _score = 0.0f;
    float _maxScore = 0.0f;
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