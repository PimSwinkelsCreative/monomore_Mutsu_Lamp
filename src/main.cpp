#include "Button.h"
#include "batteryCharging.h"
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
uint64_t lastButtonaction = 0;
uint16_t buttonTimeoutInterval = 3000; // 3 seconds

uint32_t lastStatemachineUpdate = 0;
uint16_t statemachineUpdateInterval = 10; // update at 100Hz

uint64_t lastBatteryMeasurement = 0;
uint16_t batteryMeasurementInterval = 10000; // 10 seconds

uint64_t lastLedUpdate = 0;
uint16_t ledUpdateInterval = 10; // update at 100hz

// colors:
colorPalette currentActiveColor;
colorPalette currentColor;
colorPalette prevColor;
uint16_t currentBrightness;

byte chargerStatus = DISCONNECTED;
byte prevChargerStatus;

// battery:
uint8_t batteryPercentage = 100; // always assume full battery on startup

void setup()
{
    // general setup:
    setCpuFrequencyMhz(80);
    delay(3000);
    Serial.begin(115200);

    // setup Leds:
    setupLeds();
    updateLeds();
    setColorPalette(DAY);
    currentColor = DAY;
    currentActiveColor = DAY;

    // setup BMS:
    prevChargerStatus = chargerStatus;
    setupBatterySystem();
}

void loop()
{

    if (millis() - lastStatemachineUpdate >= statemachineUpdateInterval) {
        lastStatemachineUpdate = millis();

        // update the charger status:
        prevChargerStatus = chargerStatus;
        chargerStatus = getChargerStatus();

        // handle the state machine:
        switch (state) {
        case STANDBY:
            setBrighthness(0);
            currentColor = currentActiveColor;

            // change the lighting if the charger is connected:
            if (chargerStatus != DISCONNECTED) {
                if (chargerStatus == CONNECTED_CHARGING) {
                    // show charging animation:
                    currentColor = CHARGING;
                    setBrighthness(20);

                } else {
                    // show charging done animation:
                    currentColor = CHARGING_DONE;
                    setBrighthness(20);
                }
            }

            if (topButton.input == SHORTPRESS) {
                state = FULL_ON;
                lastButtonaction = millis();
                topButton.clearInput();
            }
            break;
        case FULL_ON:
            setBrighthness(0xFFF);
            currentColor = currentActiveColor;

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
            setBrighthness(0x666); // 40%
            currentColor = currentActiveColor;

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
            setBrighthness(0x199); // 10%
            currentColor = currentActiveColor;

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
            setBrighthness(0x28); // 1%;
            currentColor = currentActiveColor;

            if (topButton.input == SHORTPRESS) {
                state = STANDBY;
                lastButtonaction = millis();
                topButton.clearInput();
            }
            break;

        default:
            break;
        }

        // print the state changes:
        if (state != prevState) {
            Serial.println("going to state: " + String(state));
            prevState = state;
        }
    }

    // periodically update the leds:
    if (millis() - lastLedUpdate >= ledUpdateInterval) {
        lastLedUpdate = millis();
        // check if the color palette needs to be set:
        if (prevColor != currentColor) {
            setColorPalette(currentColor);
            prevColor = currentColor;
        }
        updateLeds();
    }

    // perform a battery level measurement
    if (millis() - lastBatteryMeasurement >= batteryMeasurementInterval) {
        lastBatteryMeasurement = millis();
        startBatteryMeasurement();
        Serial.println("started a battery measurement");
        Serial.flush();
    }
    // update the battery level if a new measurment is ready
    if (batteryMeasurementReady()) {
        batteryPercentage = getBatteryPercentage();
        stopBatteryMeasurement();
        Serial.println("Battery Percentage: " + String(batteryPercentage));

        chargerStatus = getChargerStatus();
        Serial.println("charger status: " + String(chargerStatus));
    }

    // update the button (needs to be done as often as possible)
    topButton.update();

    // check if the color palette needs to change:
    if (topButton.input == LONGPRESS) {
        if (currentActiveColor == DAY) {
            currentActiveColor = NIGHT;
        } else {
            currentActiveColor = DAY;
        }
        lastButtonaction = millis();
        topButton.clearInput();
    }
}