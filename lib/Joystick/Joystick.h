#ifndef JOYSTICK_H
#define JOYSTICK_H

#include <Arduino.h>

class Joystick {
  public:
    Joystick(uint8_t pinX = 13, uint8_t pinY = 12);

    void begin();
    float readX();
    float readY();

    void setMidPosition();

  private:
    uint8_t _pinX;
    uint8_t _pinY;

    float _midX;
    float _midY;
    
    float normalizeX(int raw);
    float normalizeY(int raw);
};

#endif