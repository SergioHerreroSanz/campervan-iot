#pragma once

#include <Arduino.h>
#include <tempHistory.h>

float updateFanAutoLowPower()
{
    float temp = getTemp();
    int midpoint = 38;
    float steepness = 0.25;
    float power = 1 / (1 + exp(-steepness * (temp - midpoint)));
    return power;
}

float updateFanAutoHighPower()
{
    float temp = getTemp();
    int midpoint = 33;
    float steepness = 0.4;
    float power = 1 / (1 + exp(-steepness * (temp - midpoint)));
    return power;
}