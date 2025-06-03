#pragma once

#include <Arduino.h>

#define LONGPRESSTIME 2000
#define DOUBLECLICKINTERVAL 200

#define DEBOUNCETIME 10

enum clickType { NONE, SHORTPRESS, LONGPRESS, DOUBLEPRESS };

class Button {
 private:
  byte pin;
  byte state;
  byte lastReading;
  unsigned long lastStateChange;
  unsigned long timePressed;
  unsigned long timeReleased;
  bool shortPressDetected;
  bool doubleClickStarted;
  uint8_t readButtonState();

 public:
  byte input;
  Button(byte pin);
  void init();
  void update();
  void clearInput();
};