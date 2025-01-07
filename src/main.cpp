#include "ledcontrol.h"
#include <Arduino.h>

uint16_t delayTime = 500;

void setup()
{
    delay(3000);
    Serial.begin(115200);
    setupLeds();
    setBrighness(1000);
    setWhiteLedIntensity(1000);
    setRGBWLedColor(RGBWColor16(1000, 500, 0, 1500));
    updateLeds();
}

void loop()
{
}