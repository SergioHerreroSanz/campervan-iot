#pragma once

#include <Arduino.h>
#include <tempHistory.h>

int calculatePower(float a, float b, float t0, float temp)
{
    if (temp < t0)
        return 0;

    float pwr = a * (exp(b * (temp - t0)) - 1);

        if (pwr > 100)
        return 100;
    if (pwr < 0)
        return 0;
    return pwr;
}

float updateFanAutoLowPower()
{
    return calculatePower(49.1, 0.078, 18.0, getLastTemp());
}

float updateFanAutoHighPower()
{
    return calculatePower(62.55, 0.09626, 18.0, getLastTemp());
}