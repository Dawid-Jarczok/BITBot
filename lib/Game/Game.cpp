#include "Game.h"

void Game::begin() {
    _score = 0.0f;
    _isRunning = false;

    _target->reset();
}

void Game::setLedPin(uint8_t pin) {
    _ledPin = pin;
    pinMode(_ledPin, OUTPUT);
    digitalWrite(_ledPin, LOW);
}

void Game::iterate() {
    static uint32_t lastUpdate = 0;
    if (millis() - lastUpdate < _updateInterval) return;
    lastUpdate = millis();
    _pointerX = _pointer->getX();
    _pointerY = _pointer->getY();
    _targetX = _target->getX();
    _targetY = _target->getY();
    _antyTargetX = _antyTarget->getX();
    _antyTargetY = _antyTarget->getY();
    if (!_isRunning) {
        digitalWrite(_ledPin, LOW);
        return;
    }
    _gameTime = millis() - _gameStartTime;

    if (millis() - _lastGameSpeedUpTime >= _gameSpeedUpInterval) {
        _lastGameSpeedUpTime = millis();
        
        float newTargetMaxV = _target->getMaxVelocity() + _TargetVelocityUp;
        newTargetMaxV = constrain(newTargetMaxV, _modeTargetMinVelocity[_mode], _modeTargetMaxVelocity[_mode]);
        _target->setMaxVelocity(newTargetMaxV);

        float newTargetAcc = _target->getAcceleration() + _TargetAccelerationUp;
        newTargetAcc = constrain(newTargetAcc, _modeTargetMinAcceleration[_mode], _modeTargetMaxAcceleration[_mode]);
        _target->setAcceleration(newTargetAcc);

        if (_antyTargetMode) {
            _antyTarget->setMaxVelocity(newTargetMaxV);
            _antyTarget->setAcceleration(newTargetAcc);
        }
    }

    float intervalSec = (float)_updateInterval / 1000.0f;
    // Check if target is within pointer radius
    if (_pointerX - _pointerRadius <= _targetX &&
        _pointerX + _pointerRadius >= _targetX &&
        _pointerY - _pointerRadius <= _targetY &&
        _pointerY + _pointerRadius >= _targetY) {
        _score += (float)_gameTime * 0.01f * intervalSec * pow(1.0f + (float)_mode, 2);
        _isTargetInPointer = true;
        digitalWrite(_ledPin, HIGH);
    } else {
        _isTargetInPointer = false;
        digitalWrite(_ledPin, LOW);
    }

    // Check if anty-target is within pointer radius
    if (_antyTargetMode) {
        if (_pointerX - _pointerRadius <= _antyTargetX &&
        _pointerX + _pointerRadius >= _antyTargetX &&
        _pointerY - _pointerRadius <= _antyTargetY &&
        _pointerY + _pointerRadius >= _antyTargetY) {
            _score -= (float)_gameTime * 0.02f * intervalSec * pow(1.0f + (float)_mode, 2);
        }
    }
    
    if (_gameTime >= _gameDuration) {
        stop();
    }
}

void Game::start() {
    _isRunning = true;
    _score = 0;
    _gameStartTime = millis();
    _gameTime = 0;
    _lastGameSpeedUpTime = _gameStartTime;
    _calculateGameSpeedUp();
    _pointer->setMaxVelocity(_modePointerMaxVelocity[_mode]);
    _pointer->setAcceleration(_modePointerAcceleration[_mode]);
    _target->reset();
    _target->setMaxVelocity(_modeTargetMinVelocity[_mode]);
    _target->setAcceleration(_modeTargetMinAcceleration[_mode]);
    _target->start();
    if (_antyTargetMode) {
        _antyTarget->reset();
        _antyTarget->setMaxVelocity(_modeTargetMinVelocity[_mode]);
        _antyTarget->setAcceleration(_modeTargetMinAcceleration[_mode]);
        _antyTarget->start();
    } else {
        _antyTarget->stop();
    }
}

void Game::stop() {
    _target->stop();
    _antyTarget->stop();
    _isRunning = false;

    if (_maxScore < _score) {
        _maxScore = _score;
        if (_prefs) {
            _prefs->begin("settings", false);
            _prefs->putFloat("maxScore", _maxScore);
            _prefs->end();
        }
    }
}

uint32_t Game::getGameTimeLeft() {
    if (!_isRunning) return 0;
    return _gameDuration - _gameTime;
}

void Game::setMode(uint8_t mode) {
    if (_isRunning) return;
    if (mode >= maxModes) mode = maxModes - 1;
    _mode = mode;
}

void Game::_calculateGameSpeedUp() {
    float targetVelDiff = _modeTargetMaxVelocity[_mode] - _modeTargetMinVelocity[_mode];
    float targetAccDiff = _modeTargetMaxAcceleration[_mode] - _modeTargetMinAcceleration[_mode];
    _gameSpeedUpInterval = _gameDuration / _gameSpeedUpItervals;
    _TargetVelocityUp = targetVelDiff / (float)_gameSpeedUpItervals;
    _TargetAccelerationUp = targetAccDiff / (float)_gameSpeedUpItervals;
}