#pragma once

#include <Arduino.h>
#include <tempHistory.h>
#include <powerHistory.h>

#define UPDATE_FREQUENCY 2000 // Every 2s

bool isFanOn = true;
float updateFanAutoControlPower()
{
    float temp = getTemp();
    float newPower = getPower();

    Serial.print("Interior: ");
    Serial.print(temp);
    Serial.print(" °C | Exterior: ");
    Serial.print(" | PWM: ");
    Serial.println(newPower);

    return newPower;
}
