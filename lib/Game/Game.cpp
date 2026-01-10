#include "Game.h"

void Game::begin() {
    _score = 0.0f;
    _isRunning = false;
}

void Game::iterate() {
    static uint32_t lastUpdate = 0;
    if (millis() - lastUpdate < _updateInterval) return;
    lastUpdate = millis();
    if (!_isRunning) return;
    _gameTime = millis() - _gameStartTime;

    float intervalSec = (float)_updateInterval / 1000.0f;
    if (_pointerX - _pointerRadius <= _targetX &&
        _pointerX + _pointerRadius >= _targetX &&
        _pointerY - _pointerRadius <= _targetY &&
        _pointerY + _pointerRadius >= _targetY) {
        _score += (float)_gameTime * 0.01f * intervalSec;
        _isTargetInPointer = true;
    } else {
        _isTargetInPointer = false;
    }
    
    if (_gameTime >= _gameDuration) {
        stop();
    }
}

void Game::updatePositions(float pointerX, float pointerY, float targetX, float targetY) {
    _pointerX = pointerX;
    _pointerY = pointerY;
    _targetX = targetX;
    _targetY = targetY;
}

void Game::start() {
    _isRunning = true;
    _score = 0;
    _gameStartTime = millis();
    _gameTime = 0;
}

void Game::stop() {
    _isRunning = false;
}