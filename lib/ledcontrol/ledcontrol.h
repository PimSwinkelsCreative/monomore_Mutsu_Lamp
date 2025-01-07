#pragma once
#include <Arduino.h>
#include <TLC5947.h>

#define N_RGBWLEDS 4
#define N_WHITE_LEDS 4

// define the pin mapping of the led driver:
const uint8_t rgbwLedPins[] = { 23, 22, 21, 20, 18, 17, 16, 15, 13, 12, 11, 10, 8, 7, 6, 5 };
const uint8_t whiteLedPins[] = { 19, 14, 9, 4 };

void setupLeds();

void setBrighthess(uint16_t brightness);

void setWhiteLedIntensity(uint16_t intensity);

void setRGBWLedColor( RGBWColor16 color);

void updateLeds();

void enableLedSupply(bool enable = true);
