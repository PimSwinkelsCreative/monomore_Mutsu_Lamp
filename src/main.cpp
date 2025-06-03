#include "Button.h"
#include "ledcontrol.h"
#include "pinout.h"
#include <Arduino.h>

uint16_t delayTime = 500;

enum states {
    STANDBY,
    FULL_ON,
    LED_40,
    LED_10,
    LED_1
};

Button topButton(BTN);

states state = STANDBY;
states prevState = LED_1;

// timing:
uint32_t lastButtonaction = 0;
uint32_t buttonTimeoutInterval = 3000; // 10 seconds

// colors:
colorPalette currentColor;

void setup()
{
    delay(3000);
    Serial.begin(115200);
    setupLeds();
    updateLeds();
    setDayColorPalette();
    currentColor = DAY;
}

void loop()
{
    // handle the state machine:
    switch (state) {
    case STANDBY:
        if (prevState != state) {
            setBrighthness(0);
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
            setBrighthness(0xFFF);
            enableLedSupply(true);
            updateLeds();
            prevState = state;
        }
        if (topButton.input == SHORTPRESS) {
            if (millis() - lastButtonaction < buttonTimeoutInterval) {
                state = LED_40;
            } else {
                state = STANDBY;
            }
            lastButtonaction = millis();
            topButton.clearInput();
        }
        break;
    case LED_40:
        if (prevState != state) {
            // setBrighthness(0xAAA);
            setBrighthness(0x666);  //40%
            enableLedSupply(true);
            updateLeds();
            prevState = state;
        }
        if (topButton.input == SHORTPRESS) {
            if (millis() - lastButtonaction < buttonTimeoutInterval) {
                state = LED_10;
            } else {
                state = STANDBY;
            }
            lastButtonaction = millis();
            topButton.clearInput();
        }
        break;
    case LED_10:
        if (prevState != state) {
            // setBrighthness(0x555);
            setBrighthness(0x199);  //10%
            enableLedSupply(true);
            updateLeds();
            prevState = state;
        }
        if (topButton.input == SHORTPRESS) {
            if (millis() - lastButtonaction < buttonTimeoutInterval) {
                state = LED_1;
            } else {
                state = STANDBY;
            }
            lastButtonaction = millis();
            topButton.clearInput();
        }
        break;
    case LED_1:
        if (prevState != state) {
            // setBrighthness(0x7A);
            setBrighthness(0x28);   //1%
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

    if (state != prevState) {
        Serial.println("going to state: " + String(state));
    }

    // // functions that need to be updated periodically:
    topButton.update();

    // check if the color palette needs to change:
    if (topButton.input == LONGPRESS) {
        if (currentColor == DAY) {
            Serial.println("Going to Night Mode");
            setNightColorPalette();
            currentColor = NIGHT;
        } else {
            setDayColorPalette();
            Serial.println("Going to Day Mode");
            currentColor = DAY;
        }
        lastButtonaction = millis();
        updateLeds();
        topButton.clearInput();
    }
}