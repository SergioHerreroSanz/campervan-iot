#pragma once

#include <Arduino.h>
#include <tempSensor.h>

// Definir la configuración del ventilador (usando PWM)
#define PWM_PIN 12
#define PWM_CHANNEL 0
#define PWM_FREQUENCY 1000
constexpr int PWM_RESOLUTION = 8;
#define UPDATE_FREQUENCY 2000 // Every s
#define TEMP_HYSTERESIS 1.0f  // In celsius
#define MIN_FAN_POWER 0.2f    // Percentage
#define FAN_CURVE 0.45f

enum FanMode
{
    FAN_MODE_OFF = 0,
    FAN_MODE_MANUAL = 1,
    FAN_MODE_AUTO = 2
};

FanMode fanMode = FAN_MODE_MANUAL;
uint16_t fanManualPowerTarget = pow(2, PWM_RESOLUTION - 1);
float userTempTarget = 25;
bool isFanOn = true;

void changePWM(uint16_t targetValue)
{
    ledcWrite(PWM_CHANNEL, targetValue);
}

void initFan()
{
    pinMode(PWM_PIN, OUTPUT);
    ledcSetup(PWM_CHANNEL, PWM_FREQUENCY, PWM_RESOLUTION);
    ledcAttachPin(PWM_PIN, PWM_CHANNEL);
    changePWM(fanManualPowerTarget);
}

void setFanMode(FanMode mode)
{
    fanMode = mode;
}
FanMode getFanMode()
{
    return fanMode;
}

void setFanManualPowerTarget(uint16_t power)
{
    fanManualPowerTarget = power;
    changePWM(power);
}
uint16_t getFanManualPowerTarget()
{
    return fanManualPowerTarget;
}

void setUserTempTarget(float temp)
{
    userTempTarget = temp;
}
float getUserTempTarget()
{
    return userTempTarget;
}

uint32_t lastFanTimeMeasurement = 0;
void updateFanAutoControlPower()
{
    if (millis() - lastFanTimeMeasurement > UPDATE_FREQUENCY)
    {
        lastFanTimeMeasurement = millis();

        float extTemp = getTempAverage();
        float intTemp = getTemp();
        float targetTemp = max(userTempTarget, extTemp);

        Serial.print("Interior: ");
        Serial.print(intTemp);
        Serial.print(" °C | Exterior: ");
        Serial.print(extTemp);
        Serial.print(" °C | Usuario: ");
        Serial.print(userTempTarget);
        Serial.print(" °C | Objetivo: ");
        Serial.print(targetTemp);

        uint16_t newPower = 0;
        float tempDiff = fabs(intTemp - targetTemp);
        // Temperatura interior superior a la deseada
        if (isFanOn ? intTemp > targetTemp : intTemp > targetTemp + TEMP_HYSTERESIS)
        {
            float curve = 0.1 + (1 - 0.1) / (1 + exp(-1.8 * (tempDiff - 2)));
            Serial.print(" °C | Curva: ");
            Serial.print(curve);
            newPower = (pow(2, PWM_RESOLUTION) - 1) * curve;
            isFanOn = true;
        }
        // Temperatura interior inferior a la deseada
        else if (tempDiff > TEMP_HYSTERESIS)
        {
            newPower = 0;
            isFanOn = false;
        }

        if (newPower >= 0)
        {
            Serial.print(" | PWM: ");
            Serial.println(newPower);
            changePWM(newPower);
        }
    }
}

void runFan()
{
    if (fanMode == FAN_MODE_AUTO)
    {
        updateFanAutoControlPower();
    }
}