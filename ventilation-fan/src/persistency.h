#pragma once

#include <Preferences.h>

Preferences prefs;
#define PERSISTENCY_FAN_NS "ventilationFan"
#define PERSISTENCY_FAN_MODE "fanMode"
#define PERSISTENCY_FAN_MANUAL_POWER_TARGET "fanPowerTarget"
#define PERSISTENCY_USER_TEMP_TARGET "userTempTarget"

void saveFanMode(uint8_t fanMode)
{
    prefs.begin(PERSISTENCY_FAN_NS, false);
    prefs.putInt(PERSISTENCY_FAN_MODE, fanMode);
    prefs.end();
}
uint8_t loadFanMode(uint8_t defaultValue)
{
    prefs.begin(PERSISTENCY_FAN_NS, true);
    uint8_t value = prefs.getInt(PERSISTENCY_FAN_MODE, defaultValue);
    prefs.end();
    return value;
}

void saveFanManualPowerTarget(float fanManualPowerTarget)
{
    prefs.begin(PERSISTENCY_FAN_NS, false);
    prefs.putFloat(PERSISTENCY_FAN_MANUAL_POWER_TARGET, fanManualPowerTarget);
    prefs.end();
}
float loadFanManualPowerTarget(float defaultValue)
{
    prefs.begin(PERSISTENCY_FAN_NS, true);
    float value = prefs.getFloat(PERSISTENCY_FAN_MANUAL_POWER_TARGET, defaultValue);
    prefs.end();
    return value;
}

void saveUserTempTarget(float userTempTarget)
{
    prefs.begin(PERSISTENCY_FAN_NS, false);
    prefs.putFloat(PERSISTENCY_USER_TEMP_TARGET, userTempTarget);
    prefs.end();
}
float loadUserTempTarget(float defaultValue)
{
    prefs.begin(PERSISTENCY_FAN_NS, true);
    float value = prefs.getFloat(PERSISTENCY_USER_TEMP_TARGET, defaultValue);
    prefs.end();
    return value;
}