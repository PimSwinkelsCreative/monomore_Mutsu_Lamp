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
  static byte lastSample = LOW;
  byte btnState = state;
  if (sample != lastSample) {
    lastStateChange = millis();
    lastSample = sample;
  }
  if (millis() - lastStateChange > DEBOUNCETIME) {
    btnState = sample;  //set the btns state if the button is constant long enough
  }
  return btnState;
}

void Button::update() {
  uint8_t reading = readButtonState();
  static bool longButtonPressactive = false;

  // update the state:
  if (reading && !lastReading) {
    // actions on buttonPress
    timePressed = millis();
    state = true;
    Serial.println("button pressed!");

  } else if (lastReading && !reading) {
    // actions on button release:
    timeReleased = millis();
    state = false;
     Serial.println("button released!");
    // check for a short click:
    if (timeReleased - timePressed < LONGPRESSTIME && !longButtonPressactive) {
      shortPressDetected = true;
      Serial.println("Short press detected");
      Serial.println("timeReleased: "+String(timeReleased));
      Serial.println("timePressed: "+String(timePressed));
      Serial.println("Button Press legth: "+String(timeReleased-timePressed)+"ms");
    }
    longButtonPressactive = false;
  }

  // set the short button press flag if required:
  if (shortPressDetected && !state && !doubleClickStarted &&
      millis() - timeReleased > DOUBLECLICKINTERVAL) {
    input = SHORTPRESS;
    shortPressDetected = false;
       Serial.println("button input set to short press");
  }

  uint32_t currentButtonPressLentgh = millis() - timePressed;
  if (state && currentButtonPressLentgh > DOUBLECLICKINTERVAL&& currentButtonPressLentgh<(millis() - timeReleased)) {
    input = LONGPRESS;
    longButtonPressactive = true;
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
  timeReleased = millis();
  input = NONE;
}