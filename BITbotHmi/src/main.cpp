#include <Arduino.h>
#include <OneButton.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include "UARTCommandParser.h"
#include "config.h"

Adafruit_SSD1306 display(128, 64, &Wire, -1);
UARTCommandParser game(Serial1);
OneButton button1(BUTTON1_PIN, true);
OneButton button2(BUTTON2_PIN, true);
OneButton button3(BUTTON3_PIN, true);
OneButton button4(BUTTON4_PIN, true);

void button1Click();
void button2Click();
void button3Click();
void button4Click();
void setupGameHandlers();
void sendGameMsg(const char* msg, int32_t value);

int32_t score = 0;
int32_t maxScore = 0;
uint8_t mode = 0;
bool gameRunning = false;


void setup() {
    Serial.begin(115200);
    Serial.println("BITBot Starting...");

    Serial1.setPins(RX_PIN, TX_PIN);
    Serial1.begin(115200);
    game.begin();
    setupGameHandlers();

    button1.setDebounceMs(50);
    button2.setDebounceMs(50);
    button3.setDebounceMs(50);
    button4.setDebounceMs(50);

    button1.attachClick(button1Click);
    button2.attachClick(button2Click);
    button3.attachClick(button3Click);
    button4.attachClick(button4Click);

    Wire.begin(SDA_PIN, SCL_PIN);
    while (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        Serial.println(F("SSD1306 allocation failed"));
        delay(1000);
    }
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println("BITBot HMI Starting...");
    display.display();
    delay(1000);
}

void loop() {
    game.iterate();
    button1.tick();
    button2.tick();
    button3.tick();
    button4.tick();

    static uint32_t lastDisplayUpdate = 0;
    if (millis() - lastDisplayUpdate >= 100) {
        lastDisplayUpdate = millis();

        display.clearDisplay();
        display.setCursor(0, 0);
        display.printf("Game: %s\n", gameRunning ? "Running" : "Stopped");
        display.printf("Score: %d\tMax: %d\n", score, maxScore);
        display.printf("Mode: %d\n", mode);
        display.println("Buttons:");
        display.println("1: Start/Stop Game");
        display.println("2: Change Mode");
        // display.println("3: TBD");
        // display.println("4: TBD");
        display.display();
    }
}

void button1Click() {
    if (!gameRunning) {
        sendGameMsg("StartGame", 1);
        gameRunning = true;
    } else {
        sendGameMsg("StopGame", 1);
        gameRunning = false;
    }
}

void button2Click() {
    if (!gameRunning) {
        mode = (mode + 1) % 5;
        sendGameMsg("SetDifficulty", mode);
    }
}

void button3Click() {

}

void button4Click() {

}

void sendGameMsg(const char* msg, int32_t value) {
    Serial1.printf("%s=%d\n", msg, value);
}

void setupGameHandlers() {    
    game.setDefaultHandler([](const char *cmd, const char *payload){
        Serial.printf("Unknown/bad: [%s] -> [%s]\n", cmd ? cmd : "<null>", payload ? payload : "");
    });
    game.on("Hello", [](int32_t v) {
        Serial.printf("Hello command received with value: %d\n", v);
    });
    game.on("Score", [](int32_t v) {
        score = v;
    });
    game.on("MaxScore", [](int32_t v) {
        maxScore = v;
    });
    game.on("Difficulty", [](int32_t v) {
        Serial.printf("Difficulty set to %d from HMI command\n", v);
        mode = v;
    });
    game.on("GameRunning", [](int32_t v) {
        gameRunning = (bool)v;
    });
}