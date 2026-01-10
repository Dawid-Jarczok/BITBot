#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <Motor.h>
#include "config.h"
#include "Joystick.h"
#include "Pointer.h"
#include "Game.h"
#include "Wire.h"
#include <Adafruit_SSD1306.h>
#include "Target.h"
#include "UARTCommandParser.h"

#define ENC_PULSES_PER_REV 44.0
#define GEARBOX_RATIO ((22.0/12.0)*(22.0/10.0)*(22.0/10.0)*(23.0/10.0))
#define LED_COUNT 64*4

Motor motorX(MOTOR_PIN_X1, MOTOR_PIN_X2, MOTOR_PIN_PWM_X, MOTOR_ENCODER_X_PIN_A, MOTOR_ENCODER_X_PIN_B);
Motor motorY(MOTOR_PIN_Y1, MOTOR_PIN_Y2, MOTOR_PIN_PWM_Y, MOTOR_ENCODER_Y_PIN_A, MOTOR_ENCODER_Y_PIN_B);
Adafruit_NeoPixel strip(LED_COUNT, LED_MATRIX_PIN, NEO_GRB + NEO_KHZ800);
Joystick joystick(JOYSTICK_X_PIN, JOYSTICK_Y_PIN);
Pointer pointer(&joystick, 16, 16);
Target target(16, 16);
Game game(&target, &pointer);
Adafruit_SSD1306 display(128, 64, &Wire, -1);
UARTCommandParser hmi(Serial);

IRAM_ATTR void motorXEncoder() {
    motorX.updatePosition();
}

IRAM_ATTR void motorYEncoder() {
    motorY.updatePosition();
}

void setMatrixLed(uint16_t x, uint16_t y, uint32_t color);
void sendHmiMsg(const char* msg, int32_t value);
void sendHmiInit();
void sendHmiUpdate();
void setupHmiHandlers();

void setup() {
    Serial.begin(115200);
    Serial.println("BITBot Starting...");

    Serial1.setPins(RX_PIN, TX_PIN);
    Serial1.begin(115200);
    hmi.begin();
    setupHmiHandlers();

    Wire.begin(SDA_PIN, SCL_PIN);
    while (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        Serial.println(F("SSD1306 allocation failed"));
        delay(1000);
    }
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println("BITBot Starting...");
    display.display();

    joystick.begin();
    joystick.setMidPosition();

    pointer.begin();
    pointer.setUpdateInterval(10);

    target.begin();
    target.setUpdateInterval(10);

    game.begin();
    game.setLedPin(TARGET_LED_PIN);
    game.setMode(0);


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

    display.clearDisplay();
    display.setCursor(0, 0);
    display.display();

    sendHmiInit();

    game.start();
}

void loop() {
    motorX.iterate();
    pointer.iterate();
    target.iterate();

    game.updatePositions(pointer.getX(), pointer.getY(), target.getX(), target.getY());
    game.iterate();

    motorX.setSetpoint(pointer.getX() * 1.3f);

    static uint32_t lastPrint = 0;
    if (millis() - lastPrint > 100) {
        lastPrint = millis();

        hmi.iterate();
        sendHmiUpdate();

        // Serial.printf("dM X: %.2f\tPointer X: %.2f (%.2f)\tY: %.2f (%.2f)\tx:%d y:%d\t Game Score: %.2f\ttime: %dms\t%.2f %.2f\n", 
        //     pointer.getX() - motorX.getPositionMM() / 1.3f,
        //     pointer.getX(), pointer.getXVelocity(),
        //     pointer.getY(), pointer.getYVelocity(),
        //     pointer.getXInt(), pointer.getYInt(),
        //     game.getScore(), game.getGameTime(),
        //     target.getMaxVelocity(), target.getAcceleration());

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

        static uint16_t xTarget = 1, yTarget = 1;
        uint16_t xTarget_new = target.getXInt();
        uint16_t yTarget_new = target.getYInt();
        if (xTarget != xTarget_new || yTarget != yTarget_new) {
            setMatrixLed(xTarget, yTarget, strip.Color(0, 0, 0));
            xTarget = xTarget_new;
            yTarget = yTarget_new;
            setMatrixLed(xTarget, yTarget, strip.Color(50, 0, 0));
            strip.show();
        }

        display.clearDisplay();
        display.setCursor(0, 0);
        display.setTextSize(2);
        if (game.isRunning()){
            display.printf("Score: \n   %d\nTime:\n   %.1fs\n", (uint32_t)game.getScore(), game.getGameTimeLeft() / 1000.0f);
        } else {
            display.printf("Final Score:\n   %d\nMax Score:\n   %d\n", (uint32_t)game.getScore(), (uint32_t)game.getMaxScore());
        }
        display.display();

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

void sendHmiMsg(const char* msg, int32_t value) {
    Serial1.printf("%s=%d\n", msg, value);
}

void sendHmiUpdate() {
    sendHmiMsg("Score", (int32_t)game.getScore());
    sendHmiMsg("", 1);
}

void sendHmiInit() {
    sendHmiMsg("Difficulty", (int32_t)game.getMode());
}

void setupHmiHandlers() {
    hmi.setDefaultHandler([](const char *cmd, const char *payload){
        Serial.printf("Unknown/bad: [%s] -> [%s]\n", cmd ? cmd : "<null>", payload ? payload : "");
    });
    hmi.on("Hello", [](int32_t v) {
        Serial.printf("Hello command received with value: %d\n", v);
    });
    hmi.on("StartGame", [](int32_t v) {
        Serial.println("Starting game from HMI command");
        game.start();
    });
    hmi.on("StopGame", [](int32_t v) {
        Serial.println("Stopping game from HMI command");
        game.stop();
    });
    hmi.on("SetDifficulty", [](int32_t v) {
        Serial.printf("Setting game difficulty to %d from HMI command\n", v);
        game.setMode((uint8_t)v);
    });
}