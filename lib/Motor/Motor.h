#pragma once 

#include <Arduino.h>
//#include <QuidkPID.h>

class Motor {
public:
    Motor(uint8_t pin1, uint8_t pin2, uint8_t pwmPin, uint8_t encoderPinA, uint8_t encoderPinB);
    
    bool begin(float encoderPulsesPerRev, float gearboxRatio);
    void setSpeed(int16_t speed);
    void stop();
    inline int32_t getPosition() { return _encPos; }
    float getPositionRev();

    IRAM_ATTR void updatePosition();

private:
    uint8_t _pin1;
    uint8_t _pin2;
    uint8_t _pwmPin;
    uint8_t _encoderPinA;
    uint8_t _encoderPinB;
    float _encoderPulsesPerRev;
    float _gearboxRatio;

    int16_t _speed = 0;

    float _posRev = 0.0;

    volatile int32_t _encPos = 0;
    volatile uint8_t _encLastState = 0b00;

    
    float Kp = 500.0f;
    float Ki = 0.0f;
    float Kd = 15.0f;

    // QuickPID pid(&input, &output, &setpoint, Kp, Ki, Kd,
    //             QuickPID::pMode::pOnError, QuickPID::dMode::dOnMeas, QuickPID::iAwMode::iAwCondition,
    //             QuickPID::Action::direct);
};