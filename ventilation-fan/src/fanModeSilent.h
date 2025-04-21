#pragma once

#include <Arduino.h>
#include <tempHistory.h>
#include <powerHistory.h>
#include <fanModeAuto.h>

#define UPDATE_FREQUENCY 2000 // Every 2s
#define TEMP_HYSTERESIS 1.0f

bool isFanOn = true;
bool reducingTempMode = true;
float exteriorTempEstimate = 0.0f;

float reduceTemp()
{
    float currentTemp = getLastTemp();
    float power1 = updateFanAutoLowPower(currentTemp);
    float power2 = getAveragePowerLastHour() * 1.2f;
    float power3 = getLastPower();
    float maxPower = fmax(fmax(power1, power2), power3);
    float slope5min = computeTempSlope(5);

    if (reducingTempMode && slope5min < 0.1f && slope5min > -0.1f)
    {
        reducingTempMode = false;
        exteriorTempEstimate = currentTemp;
    }
    else
    {
        reducingTempMode = true;
    }

    return constrain(maxPower, 0.0f, 1.0f);
}

float maintainTemp()
{
    float temp = getLastTemp();
    float power = getlastPower();
    float avgPower = getAveragePower(5);
    float delta = getTemp(5) - temp;

    return constrain(power * (1 + delta), 0.0f, 1.0f)
}

float updateFanSilentPower()
{
    float newPower = getLastPower();
    float minLastHour = min(exteriorTempEstimate, getMinTemp());

    if (reducingTempMode || currentTemp >= minLastHour + TEMP_HYSTERESIS)
    {
        newPower = reduceTemp();
    }
    else if (currentTemp <= userTargetTemp)
    {
        newPower = 0.0f
    }
    else
    {
        newPower = maintainTemp();
    }

    Serial.print("Temp interior: ");
    Serial.print(temp);
    Serial.print(" °C | Pow: ");
    Serial.println(newPower);

    return newPower;
}
