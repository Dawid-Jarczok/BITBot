#include "Joystick.h"

Joystick::Joystick(uint8_t pinX, uint8_t pinY): _pinX(pinX), _pinY(pinY) {}

void Joystick::begin() {
  pinMode(_pinX, INPUT);
  pinMode(_pinY, INPUT);
}

void Joystick::setMidPosition() {
  long sumX = 0;
  long sumY = 0;

  const int samples = 10;

  for (int i = 0; i < samples; i++) {
    sumX += analogRead(_pinX);
    sumY += analogRead(_pinY);
    delay(10);
  }

  _midX = int(sumX / samples);
  _midY = int(sumY / samples);
}

float Joystick::normalizeX(int raw) {
  float value = raw - _midX;
  float maxVal = 4095 - _midX;

  float valueNormalized;

  if (value > 0){
    if (value > maxVal)
        return 10;
    valueNormalized = (value * 10) / maxVal;
  }
  else {
    if (value < (-_midX))
        return -10;
    valueNormalized = (value * 10) / _midX;
  }

    if (valueNormalized < 0.1 && valueNormalized > -0.1) {
      valueNormalized = 0;
    }
    return valueNormalized;
}

float Joystick::normalizeY(int raw) {
  float value = raw - _midY;
  float maxVal = 4095 - _midY;

  float valueNormalized;

  if (value > 0){
    if (value > maxVal)
        return 10;
    valueNormalized = (value * 10) / maxVal;
  }
  else {
    if (value < (-_midY))
        return -10;
    valueNormalized = (value * 10) / _midY;
  }

  if (valueNormalized < 0.2 && valueNormalized > -0.2) {
      valueNormalized = 0;
    }
    return valueNormalized;
}

float Joystick::readX() {
  float rawX = analogRead(_pinX);
  float x = normalizeX(rawX);
  return x;
}

float Joystick::readY() {
  float rawY = analogRead(_pinY);
  float y = normalizeY(rawY);
  return y;
}

