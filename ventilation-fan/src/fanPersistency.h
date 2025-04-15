#pragma once;

#include <Preferences.h>

Preferences prefs;
#define FAN_NS "fan";

void saveFanMode(uint8_t fanMode)
{
    prefs.begin(FAN_NS, false);
    prefs.putInt("fanMode", fanMode);
    prefs.end();
}
uint8_t loadFanMode(uint8_t defaultValue)
{
    prefs.begin(FAN_NS, true);
    uint8_t value = prefs.getInt("fanMode", defaultValue);
    prefs.end();
    return value;
}

void saveFanManualPowerTarget(uint16_t fanManualPowerTarget)
{
    prefs.begin(FAN_NS, false);
    prefs.putInt("fanManualPowerTarget", fanManualPowerTarget);
    prefs.end();
}
uint16_t loadFanManualPowerTarget(uint16_t defaultValue)
{
    prefs.begin(FAN_NS, true);
    uint16_t value = prefs.getInt("fanManualPowerTarget", defaultValue);
    prefs.end();
    return value;
}

void saveUserTempTarget(float userTempTarget)
{
    prefs.begin(FAN_NS, false);
    prefs.putFloat("userTempTarget", userTempTarget);
    prefs.end();
}
float loadUserTempTarget(float defaultValue)
{
    prefs.begin(FAN_NS, true);
    uint8_t value = prefs.getFloat("userTempTarget", defaultValue);
    prefs.end();
    return value;
}