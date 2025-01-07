#include "Button.h"
#include "ledcontrol.h"
#include "pinout.h"
#include <Arduino.h>

uint16_t delayTime = 500;

enum states {
    STANDBY,
    FULL_ON,
    LED_66,
    LED_33,
    LED_3
};

Button topButton(BTN);

states state = STANDBY;
states prevState = LED_3;

// timing:
uint32_t lastButtonaction = 0;
uint32_t buttonTimeoutInterval = 3000; // 10 seconds

void setup()
{
    delay(3000);
    Serial.begin(115200);
    setupLeds();
    setBrighthess(1000);
    setWhiteLedIntensity(1000);
    setRGBWLedColor(RGBWColor16(1000, 500, 0, 1500));
    updateLeds();
}

void loop()
{
    // handle the state machine:
    switch (state) {
    case STANDBY:
        if (prevState != state) {
            setBrighthess(0);
            enableLedSupply(false);
            updateLeds();
            prevState = state;
        }
        if (topButton.input == SHORTPRESS) {

            state = FULL_ON;
            lastButtonaction = millis();
            topButton.clearInput();
        }
        break;
    case FULL_ON:
        if (prevState != state) {
            setBrighthess(0xFFF);
            enableLedSupply(true);
            updateLeds();
            prevState = state;
        }
        if (topButton.input == SHORTPRESS) {
            if (millis() - lastButtonaction < buttonTimeoutInterval) {
                state = LED_66;
            } else {
                state = STANDBY;
            }
            lastButtonaction = millis();
            topButton.clearInput();
        }
        break;
    case LED_66:
        if (prevState != state) {
            setBrighthess(0xAAA);
            enableLedSupply(true);
            updateLeds();
            prevState = state;
        }
        if (topButton.input == SHORTPRESS) {
            if (millis() - lastButtonaction < buttonTimeoutInterval) {
                state = LED_33;
            } else {
                state = STANDBY;
            }
            lastButtonaction = millis();
            topButton.clearInput();
        }
        break;
    case LED_33:
        if (prevState != state) {
            setBrighthess(0x555);
            enableLedSupply(true);
            updateLeds();
            prevState = state;
        }
        if (topButton.input == SHORTPRESS) {
            if (millis() - lastButtonaction < buttonTimeoutInterval) {
                state = LED_3;
            } else {
                state = STANDBY;
            }
            lastButtonaction = millis();
            topButton.clearInput();
        }
        break;
    case LED_3:
        if (prevState != state) {
            setBrighthess(0x7A);
            enableLedSupply(true);
            updateLeds();
            prevState = state;
        }
        if (topButton.input == SHORTPRESS) {

            state = STANDBY;
            lastButtonaction = millis();
            topButton.clearInput();
        }
        break;

    default:
        break;
    }

    // // functions that need to be updated periodically:
    topButton.update();
}