#pragma once
#include <Arduino.h>

float Noise2(float x, float y);
float SmoothNoise2(float x, float y);

float CosineInterpolate(float a, float b, float x);

float Interpolate(float a, float b, float x);

float InterpolatedNoise2(float x, float y);

float PerlinNoise2(float x, float y, float persistance, int octaves);