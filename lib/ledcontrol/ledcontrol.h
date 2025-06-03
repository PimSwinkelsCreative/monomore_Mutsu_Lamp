#pragma once
#include <Arduino.h>
#include <TLC5947.h>

#define N_RGBWLEDS 4
#define N_WHITE_LEDS 4

enum colorPalette {
    DAY,
    NIGHT,
    CHARGING,
    CHARGING_DONE
};

enum animation {
    STATIC,
    GLOW,
    CHARGE
};

// define the pin mapping of the led driver:
const uint8_t rgbwLedPins[] = { 23, 22, 21, 20, 18, 17, 16, 15, 13, 12, 11, 10, 8, 7, 6, 5 };
const uint8_t whiteLedPins[] = { 19, 14, 9, 4 };

uint16_t applyBrightnessChargingAnimation(uint16_t fadeTime, uint16_t timeBetweenFades);

uint16_t applyBrightnessGlowingAnimation(float speed, float depth);

void setupLeds();

void setBrighthness(uint16_t brightness);

void setWhiteLedIntensity(uint16_t intensity);

void setColorPalette(byte palette);

void setRGBWLedColor(RGBWColor16 color);

void updateLeds();

void enableLedSupply(bool enable = true);
