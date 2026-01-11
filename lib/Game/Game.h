#pragma once

#include <Arduino.h>
#include "Target.h"
#include "Pointer.h"
#include <Preferences.h>

class Game {
public:
    Game(Target *target, Target *antyTarget, Pointer *pointer, Preferences *prefs) : _target(target), _antyTarget(antyTarget), _pointer(pointer), _prefs(prefs) {};

    void begin();
    void setLedPin(uint8_t pin);
    void iterate();
    void start();
    void stop();

    void setPointerRadius(float radius) { _pointerX = radius; }
    void setUpdateInterval(uint16_t intervalMs) { _updateInterval = intervalMs; }
    void setGameDuration(uint32_t durationMs) { _gameDuration = durationMs; }
    inline void setMaxScore(float maxScore) { _maxScore = maxScore; }
    inline void setAntyTargetMode(bool enable) { _antyTargetMode = enable; }

    inline float getScore() { return _score; }
    inline float getMaxScore() { return _maxScore; }
    inline uint32_t getGameTime() { return _gameTime; }
    inline bool isTargetInPointer() { return _isTargetInPointer; }
    inline bool isRunning() { return _isRunning; }
    inline bool getAntyTargetMode() { return _antyTargetMode; }
    uint32_t getGameTimeLeft();

    void setMode(uint8_t mode);
    inline uint8_t getMode() { return _mode; }

private:
    bool _isRunning = false;
    bool _antyTargetMode = false;
    float _score = 0.0f;
    float _maxScore = 0.0f;
    uint32_t _gameStartTime = 0;
    uint32_t _gameTime = 0;
    uint32_t _gameDuration = 30000;

    uint16_t _updateInterval = 10;

    uint8_t _ledPin = -1;

    float _targetX = 0.0f;
    float _targetY = 0.0f;
    float _antyTargetX = 0.0f;
    float _antyTargetY = 0.0f;
    float _pointerX = 0.0f;
    float _pointerY = 0.0f;
    float _pointerRadius = 0.75f;
    bool _isTargetInPointer = false;

    Target *_target = nullptr;
    Target *_antyTarget = nullptr;
    Pointer *_pointer = nullptr;
    Preferences *_prefs = nullptr;

    uint8_t _mode = 0;
    const uint8_t maxModes = 5;
    const float _modeTargetMinVelocity[5] = {0.5f, 1.5f, 3.0f, 5.5f, 12.0f};
    const float _modeTargetMaxVelocity[5] = {1.0f, 4.0f, 9.0f, 12.0f, 18.0f};
    const float _modeTargetMinAcceleration[5] = {1.0f, 1.5f, 2.0f, 4.0f, 10.0f};
    const float _modeTargetMaxAcceleration[5] = {1.5f, 3.0f, 6.0f, 9.0f, 15.0f};

    const float _modePointerMaxVelocity[5] = {2.0f, 5.0f, 10.0f, 15.0f, 20.0f};
    const float _modePointerAcceleration[5] = {2.0f, 5.0f, 8.0f, 10.0f, 15.0f};

    float _TargetVelocityUp = 0.1f;
    float _TargetAccelerationUp = 0.1f;
    const uint16_t _gameSpeedUpItervals = 5;
    uint16_t _gameSpeedUpInterval = 0; // ms
    uint32_t _lastGameSpeedUpTime = 0;

    void _calculateGameSpeedUp();
    

};