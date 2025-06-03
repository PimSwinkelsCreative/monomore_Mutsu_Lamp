
#pragma once
#include "pinout.h"
#include <Arduino.h>

#define BATTERY_MEASUREMENT_SETTLING_TIME 10 // 10ms for the LPF to stabilize
#define NUM_BATTERY_READINGS 10 // always take the average of 10 reading

#define FULL_SCALE_ADC_VOLTAGE 3.0
#define VOLTAGE_DIVIDER_RATIO 2.25 // voltage is divided by two. value is slightly increased to compensate for adc input impedance

#define BATTERY_FULL_VOLTAGE 4.2
#define BATTERY_EMPTY_VOLTAGE 3.2

enum CHARGERSTATUS {
    DISCONNECTED,
    CONNECTED_CHARGING,
    CONNECTED_FULL
};

void setupBatterySystem();

void startBatteryMeasurement();

void stopBatteryMeasurement();

bool batteryMeasurementReady();

float getBatteryVoltage();

byte getChargerStatus();

uint8_t getBatteryPercentage();