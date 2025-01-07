#include "Button.h"
Button::Button(byte pin) {
  this->pin = pin;
  lastReading = LOW;
  input = NONE;
  init();
}
void Button::init() {
  pinMode(pin, INPUT);
  update();
}

uint8_t Button::readButtonState() {
  uint8_t sample = !digitalRead(pin);  // button is low active
  if (sample != lastSample) {
    lastStateChange = millis();
    lastSample = sample;
  }
  if (millis() - lastStateChange > DEBOUNCETIME) {
    return sample;
  } else {
    return state;  // bit dodgy, look for better solution
  }
}

void Button::update() {
  uint8_t reading = readButtonState();

  // update the state:
  if (reading && !lastReading) {
    // actions on buttonPress
    timePressed = millis();
    state = true;

  } else if (lastReading && !reading) {
    // actions on button release:
    timeReleased = millis();
    state = false;
    // check for a short click:
    if (timeReleased - timePressed < LONGPRESSTIME) {
      shortPressDetected = true;
    }
  }

  // set the short button press flag if required:
  if (shortPressDetected && !state && !doubleClickStarted &&
      millis() - timeReleased > DOUBLECLICKINTERVAL) {
    input = SHORTPRESS;
    shortPressDetected = false;
  }

  if (state && millis() - timePressed > DOUBLECLICKINTERVAL) {
    input = LONGPRESS;
  }

  // detect the start of a doubleClick
  if (shortPressDetected && state &&
      millis() - timeReleased < DOUBLECLICKINTERVAL) {
    doubleClickStarted = true;
    shortPressDetected = false;
  }

  // set the double click flag
  if (shortPressDetected && doubleClickStarted) {
    input = DOUBLEPRESS;
  }
  lastReading = reading;
}

void Button::clearInput() {
  shortPressDetected = false;
  doubleClickStarted = false;
  input = NONE;
}