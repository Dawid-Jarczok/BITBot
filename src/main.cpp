#include <Arduino.h>
#include <Motor.h>
#include "config.h"

#define ENC_PULSES_PER_REV 44.0
#define GEARBOX_RATIO ((22.0/12.0)*(22.0/10.0)*(22.0/10.0)*(23.0/10.0))
#define MOTOR_PWM_START 70

Motor motorX(MOTOR_PIN_X1, MOTOR_PIN_X2, MOTOR_PIN_PWM_X, MOTOR_ENCODER_X_PIN_A, MOTOR_ENCODER_X_PIN_B);
Motor motorY(MOTOR_PIN_Y1, MOTOR_PIN_Y2, MOTOR_PIN_PWM_Y, MOTOR_ENCODER_Y_PIN_A, MOTOR_ENCODER_Y_PIN_B);

IRAM_ATTR void motorXEncoder() {
    motorX.updatePosition();
}

IRAM_ATTR void motorYEncoder() {
    motorY.updatePosition();
}

void setup() {
    Serial.begin(115200);
    Serial.println("BITBot Starting...");

    // pinMode(JOYSTICK_X_PIN, INPUT);
    // pinMode(JOYSTICK_Y_PIN, INPUT);

    while (!motorX.begin(ENC_PULSES_PER_REV, GEARBOX_RATIO)) {
        Serial.println("Motor X initialization failed!");
        delay(1000);
    }
    // motorY.begin(ENC_PULSES_PER_REV, GEARBOX_RATIO);

    attachInterrupt(MOTOR_ENCODER_X_PIN_A, motorXEncoder, CHANGE);
    attachInterrupt(MOTOR_ENCODER_X_PIN_B, motorXEncoder, CHANGE);
    // attachInterrupt(MOTOR_ENCODER_Y_PIN_A, motorYEncoder, CHANGE);
    // attachInterrupt(MOTOR_ENCODER_Y_PIN_B, motorYEncoder, CHANGE);

    Serial.println("Starting motors");
    motorX.setSpeed(255);
    //delay(2000);
    //motorX.setSpeed(-255);
    delay(2000);
    motorX.stop();
    Serial.println("Motors stopped");
}

void loop() {
    Serial.println("Hello, BITBot!");
    delay(1000);
    Serial.printf("Motor X: %f\tMotor Y: %f\n", motorX.getPositionRev(), motorY.getPositionRev());
}