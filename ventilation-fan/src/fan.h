#pragma once

#include <Arduino.h>
#include <tempSensor.h>

// Definir la configuración del ventilador (usando PWM)
#define PWM_PIN 12
#define PWM_CHANNEL 0
#define PWM_FREQUENCY 1000
constexpr int PWM_RESOLUTION = 8;
#define UPDATE_FREQUENCY 200 // Every 20s
#define USER_TEMP_TARGET 25  // In celsius
#define TEMP_HYSTERESIS 1.0f // In celsius
#define MIN_FAN_POWER 20     // Percentage
#define FAN_CURVE 0.45f

enum FanMode
{
    FAN_MODE_MANUAL = 0,
    FAN_MODE_AUTO = 1
};

FanMode fanMode = FAN_MODE_MANUAL;
uint16_t fanManualPowerTarget = pow(2, PWM_RESOLUTION - 1);
uint8_t userTemp = 25;
bool isFanOn = true;

void initFan()
{
    pinMode(PWM_PIN, OUTPUT);
    ledcSetup(PWM_CHANNEL, PWM_FREQUENCY, PWM_RESOLUTION);
    ledcAttachPin(PWM_PIN, PWM_CHANNEL);
}

void setFanMode(FanMode mode)
{
    fanMode = mode;
}

void changePWM(uint16_t targetValue)
{
    ledcWrite(PWM_CHANNEL, targetValue);
}

void setFanManualPowerTarget(uint16_t power)
{
    fanManualPowerTarget = power;
    changePWM(power);
}

uint32_t lastFanTimeMeasurement = 0;
void updateFanAutoControlPower()
{
    if (millis() - lastFanTimeMeasurement > UPDATE_FREQUENCY)
    {
        lastFanTimeMeasurement = millis();

        float extTemp = getTempAverage();
        float intTemp = getTemp();
        float userTemp = USER_TEMP_TARGET;
        float targetTemp = min(userTemp, extTemp);

        Serial.print("📊 Interior (media): ");
        Serial.print(intTemp);
        Serial.print(" °C | Exterior estimado: ");
        Serial.print(extTemp);
        Serial.print(" °C | Usuario: ");
        Serial.print(userTemp);
        Serial.print(" °C | Objetivo: ");
        Serial.println(targetTemp);

        float tempDiff = abs(intTemp - targetTemp);
        // Temperatura interior superior a la deseada
        if (isFanOn ? intTemp > targetTemp : intTemp > targetTemp + TEMP_HYSTERESIS)
        {
            float factor = 1.0f - exp(tempDiff * FAN_CURVE);
            factor = constrain(factor, 0.0f, 1.0f);

            uint16_t maxPWM = pow(2, PWM_RESOLUTION) - 1;
            uint16_t minPWM = maxPWM * MIN_FAN_POWER / 100;

            uint16_t power = minPWM + ((maxPWM - minPWM) * factor);
            changePWM(power);
            isFanOn = true;
        }
        // Temperatura interior inferior a la deseada
        else if (tempDiff > TEMP_HYSTERESIS)
        {
            changePWM(0);
            isFanOn = false;
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