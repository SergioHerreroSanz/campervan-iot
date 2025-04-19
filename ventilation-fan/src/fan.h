#pragma once

#include <Arduino.h>
#include <persistency.h>
#include <fanModeSilent.h>
#include <fanModeAuto.h>
#include <powerHistory.h>
#include <tempHistory.h>

#define PWM_PIN 14
#define PWM_CHANNEL 0
#define PWM_FREQUENCY 25000
constexpr int PWM_RESOLUTION = 8;
#define UPDATE_FREQUENCY 2000 // Every s
#define TEMP_HYSTERESIS 1.0f  // In celsius
#define MIN_FAN_POWER 0.2f    // Percentage

#define DEFAULT_FAN_MODE = FAN_MODE_MANUAL;
#define DEFAULT_FAN_MANUAL_POWER_TARGET = 255;
#define DEFAULT_FAN_TURBO_POWER = 1.0f;
#define DEFAULT_USER_TEMP_TARGET = 25;

enum FanMode
{
    FAN_MODE_OFF = 0,
    FAN_MODE_MANUAL = 1,
    FAN_MODE_TURBO = 2,
    FAN_MODE_AUTO_LOW = 3,
    FAN_MODE_AUTO_HIGH = 4,
    FAN_MODE_SILENT = 5
};

FanMode fanMode = FAN_MODE_MANUAL;
float fanManualPowerTarget = 1;
float userTempTarget = 25.0f;
float power = 0;

void changePower(float power)
{
    addPowerToHistory(power);
    ledcWrite(PWM_CHANNEL, (pow(2, PWM_RESOLUTION) - 1) * power);
}

void initFan()
{
    pinMode(PWM_PIN, OUTPUT);
    ledcSetup(PWM_CHANNEL, PWM_FREQUENCY, PWM_RESOLUTION);
    ledcAttachPin(PWM_PIN, PWM_CHANNEL);

    fanMode = (FanMode)loadFanMode(fanMode);
    fanManualPowerTarget = loadFanManualPowerTarget(fanManualPowerTarget);
    userTempTarget = loadUserTempTarget(userTempTarget);
    
    ledcWrite(PWM_CHANNEL, (pow(2, PWM_RESOLUTION) - 1) * fanManualPowerTarget);
}

void setFanMode(FanMode mode)
{
    fanMode = mode;
    saveFanMode(mode);
}
FanMode getFanMode()
{
    return fanMode;
}

void setFanManualPowerTarget(float power)
{
    fanManualPowerTarget = power;
    saveFanManualPowerTarget(power);
}
float getFanManualPowerTarget()
{
    return fanManualPowerTarget;
}

void setUserTempTarget(float temp)
{
    userTempTarget = temp;
    saveUserTempTarget(temp);
}
float getUserTempTarget()
{
    return userTempTarget;
}

uint32_t lastFanTimeMeasurement = 0;
void runFan()
{
    if (millis() - lastFanTimeMeasurement > UPDATE_FREQUENCY)
    {
        lastFanTimeMeasurement = millis();

        if (fanMode == FAN_MODE_OFF)
        {
            changePower(0);
        }
        else if (fanMode == FAN_MODE_TURBO)
        {
            changePower(1);
        }
        else if (fanMode == FAN_MODE_MANUAL)
        {
            changePower(fanManualPowerTarget);
        }
        else if (fanMode == FAN_MODE_AUTO_LOW)
        {
            changePower(updateFanAutoLowPower());
        }
        else if (fanMode == FAN_MODE_AUTO_HIGH)
        {
            changePower(updateFanAutoHighPower());
        }
        else if (fanMode == FAN_MODE_SILENT)
        {
            changePower(updateFanAutoControlPower());
        }
    }
}