#include "batteryCharging.h"

bool batteryMeasurementActive = false;
unsigned long batteryMeasurmentStarted = 0;

void setupBatterySystem()
{
    // set the pin sin their required mode:
    pinMode(CHARGER_DETECT, INPUT);
    pinMode(LED_CHRG_N, INPUT_PULLUP);
    pinMode(LED_STDBY_N, INPUT_PULLUP);
    pinMode(BAT, INPUT);
    pinMode(BAT_MEAS_EN, OUTPUT);
    digitalWrite(BAT_MEAS_EN, LOW);
}

void startBatteryMeasurement()
{
    if (batteryMeasurementActive)
        return;
    batteryMeasurmentStarted = millis();
    digitalWrite(BAT_MEAS_EN, HIGH);
    batteryMeasurementActive = true;
}

void stopBatteryMeasurement()
{
    if (!batteryMeasurementActive)
        return;
    digitalWrite(BAT_MEAS_EN, LOW);
    batteryMeasurementActive = false;
}

bool batteryMeasurementReady()
{
    if (batteryMeasurementActive && millis() - batteryMeasurmentStarted > BATTERY_MEASUREMENT_SETTLING_TIME) {
        return true;
    }
    return false;
}

float getBatteryVoltage()
{
    float reading = 0;
    for (int i = 0; i < NUM_BATTERY_READINGS; i++) {
        reading += analogRead(BAT);
    }
    reading /= NUM_BATTERY_READINGS;
    float batVoltage = VOLTAGE_DIVIDER_RATIO * FULL_SCALE_ADC_VOLTAGE * reading / 4095.0;
    Serial.println("Battery Voltage: "+String(batVoltage));
    return batVoltage;
}

byte getChargerStatus()
{
    // first check whether the charger is connected:
    if (digitalRead(CHARGER_DETECT)) {
        if (!digitalRead(LED_CHRG_N)) {
            return CONNECTED_CHARGING;
        }
        return CONNECTED_FULL;
    }
    return DISCONNECTED;
}

uint8_t getBatteryPercentage()
{
    float volts = getBatteryVoltage();

    // TODO: create a more realistic calculation
    uint8_t percentage = constrain(uint8_t(100.0 * (volts - BATTERY_EMPTY_VOLTAGE) / (BATTERY_FULL_VOLTAGE - BATTERY_EMPTY_VOLTAGE)), 0, 100);
    return percentage;
}
