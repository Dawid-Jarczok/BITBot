#include "Motor.h"

Motor::Motor(uint8_t pin1, uint8_t pin2, uint8_t pwmPin, uint8_t encoderPinA, uint8_t encoderPinB)
    : _pin1(pin1), _pin2(pin2), _pwmPin(pwmPin), _encoderPinA(encoderPinA), _encoderPinB(encoderPinB) {
}

bool Motor::begin(float encoderPulsesPerRev, float gearboxRatio) {
    _encoderPulsesPerRev = encoderPulsesPerRev;
    _gearboxRatio = gearboxRatio;

    _enc_err_thr = (2.0 / _encoderPulsesPerRev) / _gearboxRatio;

    pinMode(_pin1, OUTPUT);
    pinMode(_pin2, OUTPUT);
    pinMode(_pwmPin, OUTPUT);
    pinMode(_encoderPinA, INPUT_PULLUP);
    pinMode(_encoderPinB, INPUT_PULLUP);

    bool res = ledcAttach(_pwmPin, 5000, 8);
    ledcWrite(_pwmPin, 0);

    pid.SetOutputLimits(-255, 255);
    pid.SetSampleTimeUs(10);
    pid.SetMode(QuickPID::Control::automatic);

    return res;
}

void Motor::setSpeed(int16_t speed) {
    constrain(speed, -255, 255);
    _speed = speed;
    if (speed > 0) {
        digitalWrite(_pin1, HIGH);
        digitalWrite(_pin2, LOW);
        ledcWrite(_pwmPin, speed);
    } else if (speed < 0) {
        digitalWrite(_pin1, LOW);
        digitalWrite(_pin2, HIGH);
        ledcWrite(_pwmPin, -speed);
    } else {
        digitalWrite(_pin1, LOW);
        digitalWrite(_pin2, LOW);
        ledcWrite(_pwmPin, 0);
    }
}

void Motor::stop() {
    setSpeed(0);
}

float Motor::getPositionRev() {
    float posRev = (float)_encPos / _encoderPulsesPerRev;
    posRev /= _gearboxRatio;
    _posRev = posRev;
    return _posRev;
}

float Motor::getPositionMM() {
    float posRev = getPositionRev();
    float posRack = posRev * PI;
    return posRack;
}

void Motor::updatePosition() {
    static const int8_t encoderTable[16] = {
        0, -1, +1, 0,
        +1, 0, 0, -1,
        -1, 0, 0, +1,
        0, +1, -1, 0
    };
    uint8_t encState = (digitalRead(_encoderPinA) << 1) | digitalRead(_encoderPinB);
    uint8_t idx = (_encLastState << 2) | encState;
    int8_t movement = encoderTable[idx];
    if (movement != 0 && _encLastState != encState) _encPos += movement;
    _encLastState = encState;
}

void Motor::iterate() {
    static uint32_t lastTime = 0;
    if (millis() - lastTime < 10) return;
    lastTime = millis();
    if (_pidEnabled) {
        input = getPositionMM();
        if (abs(setpoint - input) < 0.001f) {
            stop();
            output = 0.0f;
        } else {
            pid.Compute();
            if (output == 0.0f) stop();
            else {
                bool dir = output > 0;
                uint16_t pwm = (uint16_t)abs(output);
                pwm = map(pwm, 0, 255, 40, 255); // Ensure minimum PWM
                if (dir) setSpeed(pwm);
                else setSpeed(-pwm);
            }
        }
    }
}