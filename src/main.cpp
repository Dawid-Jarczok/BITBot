#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <Motor.h>
#include "config.h"
#include "Joystick.h"
#include "Pointer.h"

#define ENC_PULSES_PER_REV 44.0
#define GEARBOX_RATIO ((22.0/12.0)*(22.0/10.0)*(22.0/10.0)*(23.0/10.0))
#define LED_COUNT 64*4

Motor motorX(MOTOR_PIN_X1, MOTOR_PIN_X2, MOTOR_PIN_PWM_X, MOTOR_ENCODER_X_PIN_A, MOTOR_ENCODER_X_PIN_B);
Motor motorY(MOTOR_PIN_Y1, MOTOR_PIN_Y2, MOTOR_PIN_PWM_Y, MOTOR_ENCODER_Y_PIN_A, MOTOR_ENCODER_Y_PIN_B);
Adafruit_NeoPixel strip(LED_COUNT, LED_MATRIX_PIN, NEO_GRB + NEO_KHZ800);
Joystick joystick(JOYSTICK_X_PIN, JOYSTICK_Y_PIN);
Pointer pointer(&joystick, 16, 16);

IRAM_ATTR void motorXEncoder() {
    motorX.updatePosition();
}

IRAM_ATTR void motorYEncoder() {
    motorY.updatePosition();
}

void setMatrixLed(uint16_t x, uint16_t y, uint32_t color);

void setup() {
    Serial.begin(115200);
    Serial.println("BITBot Starting...");

    joystick.begin();
    joystick.setMidPosition();

    pointer.begin();
    pointer.setMaxVelocity(2.0f);
    pointer.setAcceleration(0.5f);


    while (!motorX.begin(ENC_PULSES_PER_REV, GEARBOX_RATIO)) {
        Serial.println("Motor X initialization failed!");
        delay(1000);
    }
    // while (!motorY.begin(ENC_PULSES_PER_REV, GEARBOX_RATIO)) {
    //     Serial.println("Motor Y initialization failed!");
    //     delay(1000);
    // }

    strip.begin();
    strip.show();
    strip.setBrightness(10);

    attachInterrupt(MOTOR_ENCODER_X_PIN_A, motorXEncoder, CHANGE);
    attachInterrupt(MOTOR_ENCODER_X_PIN_B, motorXEncoder, CHANGE);
    // attachInterrupt(MOTOR_ENCODER_Y_PIN_A, motorYEncoder, CHANGE);
    // attachInterrupt(MOTOR_ENCODER_Y_PIN_B, motorYEncoder, CHANGE);

    Serial.println("Starting motors");
    motorX.setPID(true);
    motorX.setSetpoint(0.0);
    // motorY.setPID(true);
    // motorY.setSetpoint(0.0);

    Serial.println("Motors stopped");
    Serial.println("Setup complete");
}

void loop() {
    motorX.iterate();
    pointer.iterate();

    static uint32_t lastPrint = 0;
    if (millis() - lastPrint > 100) {
        lastPrint = millis();
        //Serial.printf("Motor X: %.3f\tMotor Y: %.3f\n", motorX.getPositionRev(), motorY.getPositionRev());

        Serial.printf("Pointer X: %.2f (%.2f)\tY: %.2f (%.2f)\tx:%d y:%d\n", 
            pointer.getX(), pointer.getXVelocity(),
            pointer.getY(), pointer.getYVelocity(),
            pointer.getXInt(), pointer.getYInt());

        static uint16_t xLed = 1, yLed = 1;
        uint16_t xLed_new = pointer.getXInt();
        uint16_t yLed_new = pointer.getYInt();
        if (xLed != xLed_new || yLed != yLed_new) {
            setMatrixLed(xLed, yLed, strip.Color(0, 0, 0));
            xLed = xLed_new;
            yLed = yLed_new;
            setMatrixLed(xLed, yLed, strip.Color(0, 50, 0));
            strip.show();
        }
    }
}

void setMatrixLed(uint16_t x, uint16_t y, uint32_t color) {
    if (x > 15 || y > 15) return;
    uint16_t index = 0;
    if (x >= 8) {
        x -= 8;
        index += 128;
    }
    index += x + (y * 8);
    strip.setPixelColor(index, color);
}