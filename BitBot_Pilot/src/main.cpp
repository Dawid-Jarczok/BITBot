#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "OneButton.h"
#include "config.h" 
#include "UARTCommandParser.h"

// --- EKRAN ---
Adafruit_SSD1306 display(128, 64, &Wire1, -1);
UARTCommandParser hmi(Serial1);

int32_t robotScore = 0;
int32_t robotDifficulty = 0;

// --- INICJALIZACJA PRZYCISKÓW ---
OneButton btnUP(PIN_UP, true, true);
OneButton btnDown(PIN_DOWN, true, true);
OneButton btnOk(PIN_OK, true, true);
OneButton btnBack(PIN_BACK, true, true);


// --- LOGIKA MENU ---
enum AppState {
    MENU,
    GAME_RUNNING,
    GAME_INFO,
    DIFFICULTY_INFO,
    PAUSE,
    RECORD,
};

AppState currentState = MENU;
AppState previousState = MENU; // Aby wrócić z PAUSE do gry

// Opcje menu
const char* menuItems[] = {
    "START GAME",
    "BEST SCORE",
    "GAME INFO",
    "DIFFICULTY LEVEL",
};
int menuLength = 4;
int selectedOption = 0;

// --- ZMIENNE GRY ---
uint32_t gameStartTime = 0;
uint32_t gameScore = 0;
uint32_t maxScore = 0; // Rekord

// --- DEKLARACJE FUNKCJI ---
void drawMenu();
void drawGame();
void drawInfo();
void drawDifficultyInfo();
void drawPause();
void drawRecord();
void actionUP();
void actionDown();
void actionOk();
void actionBack();
void initGame();
void stopGame();
void sendHmiMsg(const char* msg, int32_t value);
void setupHmiHandlers();


// --- SETUP ---
void setup() {
    Serial.begin(115200);
    
    // 1. Start UART do Robota
    Serial1.setPins(PILOT_RX_PIN, PILOT_TX_PIN);
    Serial1.begin(115200);
    hmi.begin();
    setupHmiHandlers();


    // 1. Ekran
    Wire1.begin(SDA_PIN, SCL_PIN);
    if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
        Serial.println(F("OLED Error")); 
        for(;;);
    }
    
    // 2. Przyciski
    btnUP.attachClick(actionUP);
    btnDown.attachClick(actionDown);
    btnOk.attachClick(actionOk);
    btnBack.attachClick(actionBack); 
    
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println(F("BitBot Initializing..."));
    display.display();
    delay(1000);

    // Ping testowy
    sendHmiMsg("Hello", 1);
}

void loop() {
    // 1. Odbieranie danych z Robota (non-stop)
    hmi.iterate();

    btnUP.tick();
    btnDown.tick();
    btnOk.tick();
    btnBack.tick();

    // 3. Rysowanie w zależności od stanu
    static uint32_t lastDraw = 0;
    if (millis() - lastDraw > 50) { // 20 FPS
        lastDraw = millis();
        display.clearDisplay();

        switch (currentState) {
            case MENU:
                drawMenu();
                break;
            case GAME_RUNNING:
                drawGame();
                break;
            case GAME_INFO:
                drawInfo();
                break;
            case PAUSE:
                drawPause();
                break;
            case RECORD:
                drawRecord();
                break;
            case DIFFICULTY_INFO:
                drawDifficultyInfo();
                break;
        }
        display.display();
    }
}

// --- FUNKCJE RYSOWANIA ---

void drawMenu() {
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println(F("--- MAIN MENU ---"));
    display.drawLine(0, 10, 128, 10, SSD1306_WHITE);

    for (int i = 0; i < menuLength; i++) {
        int y = 20 + (i * 10);

        if (i == selectedOption) {
            display.fillRect(0, y - 1, 128, 10, SSD1306_WHITE);
            display.setTextColor(SSD1306_BLACK);
            display.setCursor(2, y);
            display.print("> "); 
            display.println(menuItems[i]);
        } else {
            display.setTextColor(SSD1306_WHITE);
            display.setCursor(2, y);
            display.print("  "); 
            display.println(menuItems[i]);
        }
    }
}

void drawGame() {
    display.setTextSize(1.4);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(10, 5);
    display.println(F("GAME RUNNING"));
    
    display.setTextSize(1);
    uint32_t elapsedTime = (millis() - gameStartTime) / 1000;
    display.setCursor(10, 25);
    display.print("Time: ");
    display.print(elapsedTime);
    display.println(" s");

    display.setCursor(10, 35);
    display.print("Level: ");
        switch (robotDifficulty)
    {
    case 0:
        display.print("Easy");
        break;
    case 1:
        display.print("Medium");
        break;
    case 2:
        display.print("Hard");
        break;  
    default:
        display.print(robotDifficulty);
        break;
    }

    
    display.drawLine(0, 48, 128, 48, SSD1306_WHITE);
    display.setCursor(0, 54);
    display.println(F("* : menu"));
}

void drawInfo() {
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println(F("--- GAME INFO ---"));
    display.drawLine(0, 10, 128, 10, SSD1306_WHITE);

    display.setCursor(0, 15);
    display.println(F("BITBot v1.0"));
    display.println(F("Hackathon Build"));
    display.println(F("Team: Wymiatacze"));
    display.println();
    display.println(F("Press BACK *"));
    display.println(F("to return to menu"));
}

void drawRecord() {
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println(F("--- RECORD ---"));
    display.drawLine(0, 10, 128, 10, SSD1306_WHITE);
    display.setCursor(0, 20);
    display.println(F("Best score:"));
    display.setTextSize(2);
    display.setCursor(20, 35);
    display.println(maxScore);
    display.setTextSize(1);
    display.setCursor(0, 54);
    display.println(F("BACK: return to menu"));
}

void drawPause() {
    display.setTextSize(1.5);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(20, 15);
    display.println(F("Are you sure"));
    display.println(F("you want to quit?"));
    display.setTextSize(1);
    display.drawLine(0, 35, 128, 35, SSD1306_WHITE);
    display.setCursor(0, 45);
    display.println(F("#: No *: Yes"));
}

void drawDifficultyInfo() {
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println(F("-- DIFFICULTY LEVEL --"));
    display.drawLine(0, 10, 128, 10, SSD1306_WHITE);
    
    display.setCursor(0, 20);
    display.println(F("Current level:"));
    
    // Duży, ładny napis poziomu
    display.setTextSize(2);
    display.setCursor(10, 35);
    
    switch (robotDifficulty)
    {
    case 0:
        display.print("EASY");
        break;
    case 1:
        display.print("MEDIUM");
        break;
    case 2:
        display.print("HARD");
        break;  
    default:
        display.print(robotDifficulty);
        break;
    }

    display.setTextSize(1);
    display.setCursor(0, 54);
    display.println(F("#: CHANGE   *: BACK"));
}


// --- AKCJE PRZYCISKÓW ---

void actionUP() {
    if (currentState == MENU) {
        selectedOption--;
        if (selectedOption < 0) {
            selectedOption = menuLength - 1;
        }
    }
}

void actionDown() {
    if (currentState == MENU) {
        selectedOption++;
        if (selectedOption >= menuLength) {
            selectedOption = 0;
        }
    }
}

void actionOk() {
    if (currentState == MENU) {
        switch (selectedOption) {
            case 0: // Start Gry
                initGame();
                currentState = GAME_RUNNING;
                break;
            case 1: // Rekord
                currentState = RECORD;
                break;
            case 2: // Info
                currentState = GAME_INFO;
                break;
            case 3: // Przycski
                currentState = DIFFICULTY_INFO;
                break;
        }
    }else if (currentState == DIFFICULTY_INFO) {
        // 1. Obliczamy następny poziom
        int nextDiff = robotDifficulty + 1;
        if (nextDiff > 2) nextDiff = 0;
        
        // 2. Wysyłamy do robota (żeby wiedział)
        sendHmiMsg("SetDifficulty", nextDiff);

        // Aktualizujemy zmienną lokalną
        robotDifficulty = nextDiff; 
        
        // Debug
        Serial.print("Zmieniono lokalnie na: ");
        Serial.println(robotDifficulty);
        } 
    else if (currentState == PAUSE) {
        // Wznów grę
        currentState = GAME_RUNNING;
    }
}

void actionBack() {
    if (currentState == GAME_RUNNING) {
        // Pauza zamiast powrotu do menu
        previousState = GAME_RUNNING;
        currentState = PAUSE;
    }
    else if (currentState == PAUSE) {
        // Z pauzy wrócić do menu
        stopGame();
        currentState = MENU;
    }
    else {
        // Z innych stanów wrócić do menu
        stopGame();
        currentState = MENU;
    }
}

// --- FUNKCJE GRY ---

void initGame() {
    gameStartTime = millis();
    robotScore = 0; // Resetujemy wizualnie
    sendHmiMsg("StartGame", 1); 
    Serial.println("Gra: Wyslalem START");
}

void stopGame() {
    sendHmiMsg("StopGame", 1); 
    Serial.println("Gra: Wyslalem STOP");
}

void setupHmiHandlers() {
    hmi.on("Score", [](int32_t v) {
        robotScore = v;
        if (robotScore > maxScore) maxScore = robotScore;
    });

    hmi.on("Difficulty", [](int32_t v) {
        robotDifficulty = v;
    });
    
    hmi.setDefaultHandler([](const char *cmd, const char *payload){
    });
}

void sendHmiMsg(const char* cmd, int32_t value) {
    Serial1.printf("%s=%d\n", cmd, value);
    Serial.printf("Wysylam: %s=%d\n", cmd, value);
}
