#include "ledcontrol.h"

#include <math.h>
#include "noise.h"

#include "pinout.h"

// #define DEBUG_LED_MAPPING

// color palettes:
// Day mode:
uint16_t dayWhiteLevel = 2000;
RGBWColor16 dayRGBWColor = RGBWColor16(2000, 1000, 0, 3000);
// night mode:
uint16_t nightWhiteLevel = 100;
RGBWColor16 nightRGBWColor = RGBWColor16(4095, 500, 0, 0);

RGBWColor16 rgbwLeds[N_RGBWLEDS];
uint16_t whiteLeds[N_WHITE_LEDS];

uint16_t leds[LEDDOTSPERDRIVER];

uint16_t maxBrightness = 0xFFF;

// create the TLC5947 object:
// The led array needs to be initialized prior to this object creation
TLC5947 ledDriver(leds, sizeof(leds) / sizeof(leds[0]), LED_SCLK, LED_SIN, LED_LATCH, LED_BLANK);

// stating:
bool ledSupplyEnabled = false;
animation currentAnimation = STATIC;
colorPalette currentColorPalette;

void setBrighthness(uint16_t brightness)
{
    maxBrightness = brightness;
    if (brightness == 0) {
        enableLedSupply(false);
    } else {
        enableLedSupply(true);
    }
}

void setupLeds()
{
    // all initialisation is done by the constructor
    // only thing tha needs to be done is nulling all values and outputting that to the driver
    setRGBWLedColor(RGBWColor16(0, 0, 0, 0));
    setWhiteLedIntensity(0);
    for (int i = 0; i < LEDDOTSPERDRIVER; i++) {
        leds[i] = 0;
    }
    updateLeds();

    // start the 24V supply:
    enableLedSupply(true);
}

void setColorPalette(byte palette)
{
    switch (palette) {
    case DAY:
        setWhiteLedIntensity(dayWhiteLevel);
        setRGBWLedColor(dayRGBWColor);
        currentAnimation = STATIC;
        currentColorPalette = DAY;
        break;
    case NIGHT:
        setWhiteLedIntensity(nightWhiteLevel);
        setRGBWLedColor(nightRGBWColor);
        currentAnimation = STATIC;
        currentColorPalette = NIGHT;
        break;
    case CHARGING:
        setWhiteLedIntensity(0);
        setRGBWLedColor(RGBWColor16(4000, 0, 0, 0));
        currentAnimation = CHARGE;
        currentColorPalette = CHARGING;
        break;
    case CHARGING_DONE:
        setWhiteLedIntensity(0);
        setRGBWLedColor(RGBWColor16(0, 4000, 0, 0));
        currentAnimation = CHARGE;
        currentColorPalette = CHARGING_DONE;
        break;

    default:
        // default to the day palette
        setWhiteLedIntensity(dayWhiteLevel);
        setRGBWLedColor(dayRGBWColor);
        break;
    }
}

void setWhiteLedIntensity(uint16_t intensity)
{
    for (int i = 0; i < N_WHITE_LEDS; i++) {
        whiteLeds[i] = intensity;
    }
}

void setRGBWLedColor(RGBWColor16 color)
{
    for (int i = 0; i < N_RGBWLEDS; i++) {
        memcpy(&rgbwLeds[i], &color, sizeof(color));
    }
}

uint16_t applyBrightnessChargingAnimation(uint16_t fadeTime, uint16_t timeBetweenFades)
{
    uint16_t totalTime = fadeTime + timeBetweenFades;
    uint16_t animationTime = millis() % totalTime;
    uint16_t brightness;
    if (animationTime < fadeTime / 2) {
        brightness = map(animationTime, 0, fadeTime / 2, 0, maxBrightness);
    } else if (animationTime < fadeTime) {
        brightness = map(animationTime, fadeTime / 2, fadeTime, maxBrightness, 0);
    } else {
        brightness = 0;
    }
    return brightness;
}

uint16_t applyBrightnessGlowingAnimation(float speed, float depth)
{
    depth = constrain(depth,0,1);
    speed = constrain(speed,0,1);
    float x1 = float(speed*millis()/100.0);
    float y1 = 10.0;
    uint16_t brightness = float(maxBrightness)*(1-(PerlinNoise2(x1,y1,0.25,3)/2.0)*depth);
    return brightness;
}

void updateLeds()
{
    uint16_t brightness = maxBrightness;
    if (currentAnimation == CHARGE) {
        brightness = applyBrightnessChargingAnimation(3000, 2000);
    } else if(currentAnimation == GLOW){
        brightness = applyBrightnessGlowingAnimation(0.9,0.9); 
    }

    // copy all data to the global led array and scale the brightness:
    for (int i = 0; i < 4 * N_RGBWLEDS; i += 4) {
        leds[rgbwLedPins[i]] = map(rgbwLeds[i / 4].r, 0, 0xFFF, 0, brightness);
        leds[rgbwLedPins[i + 1]] = map(rgbwLeds[i / 4].g, 0, 0xFFF, 0, brightness);
        leds[rgbwLedPins[i + 2]] = map(rgbwLeds[i / 4].b, 0, 0xFFF, 0, brightness);
        leds[rgbwLedPins[i + 3]] = map(rgbwLeds[i / 4].w, 0, 0xFFF, 0, brightness);
    }
    for (int i = 0; i < N_WHITE_LEDS; i++) {
        leds[whiteLedPins[i]] = map(whiteLeds[i], 0, 0xFFF, 0, brightness);
    }

#ifdef DEBUG_LED_MAPPING
    Serial.print("INDEX:\t");
    for (int i = 0; i < sizeof(leds) / sizeof(leds[0]); i++) {
        Serial.print("\t");
        Serial.print(i);
    }
    Serial.println();
    Serial.print("LED OUTPUT:");
    for (int i = 0; i < sizeof(leds) / sizeof(leds[0]); i++) {
        Serial.print("\t");
        Serial.print(leds[i]);
    }
    Serial.println();
#endif

    // output the led data:
    ledDriver.update();
}

void enableLedSupply(bool enable)
{
    if (ledSupplyEnabled == enable)
        return;
    pinMode(PWR_EN, OUTPUT);
    if (enable) {
        digitalWrite(PWR_EN, HIGH);
    } else {
        digitalWrite(PWR_EN, LOW);
    }
    ledSupplyEnabled = enable;
}

