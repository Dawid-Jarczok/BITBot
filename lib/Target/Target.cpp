#include "Target.h"

void Target::begin() {
    uint8_t spawnX, spawnY;
    spawningPoint(spawnX, spawnY);
    _x = (float)spawnX;
    _y = (float)spawnY;

    spawnNewTarget();
}

void Target::iterate() {
    static uint32_t lastUpdate = 0;
    if (millis() - lastUpdate < _updateInterfal) return;
    lastUpdate = millis();

    float intervalSec = (float)_updateInterfal / 1000.0f;

    float toTargetX = _targetX - getX();
    float toTargetY = _targetY - getY();

    float distanceToTarget = sqrtf(toTargetX * toTargetX + toTargetY * toTargetY);
    if (distanceToTarget < 0.5f) {
        spawnNewTarget();
    }

    _xVel += sign(toTargetX) * _acceleration * intervalSec;
    _yVel += sign(toTargetY) * _acceleration * intervalSec;
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

float Target::sign(float distance) {
    if (distance > 0.0f) return 1.0f;
    if (distance < 0.0f) return -1.0f;
    return 0.0f;
}

void Target::spawningPoint(uint8_t &X, uint8_t &Y){
    float Xmin = 0;
    float Xmax = 16;
    float Ymin = 0;
    float Ymax = 16;

    // float dotX = random(0, 64) * 0.25f;
    // float dotY = random(0, 64) * 0.25f;
    float dotX = random(0, 16);
    float dotY = random(0, 16);

    dotX = constrain(dotX, Xmin, Xmax);
    dotY = constrain(dotY, Ymin, Ymax);

    X = uint8_t(dotX);
    Y = uint8_t(dotY);
}

void Target::spawnNewTarget() {
    uint8_t sx, sy;
    spawningPoint(sx, sy);
    _targetX = (float)sx;
    _targetY = (float)sy;
}