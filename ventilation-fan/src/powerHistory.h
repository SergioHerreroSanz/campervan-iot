#pragma once

#define POWER_HISTORY_SIZE 60
float powerHistory[POWER_HISTORY_SIZE];

void addPowerToHistory(float t)
{
    for (uint8_t i = 0; i < POWER_HISTORY_SIZE - 1; i++)
    {
        powerHistory[i] = powerHistory[i + 1];
    }
    powerHistory[POWER_HISTORY_SIZE - 1] = t;
}

float getAveragePower(uint8_t count)
{
    float sum = 0;
    uint8_t validCount = 0;

    if (count > POWER_HISTORY_SIZE)
    {
        count = POWER_HISTORY_SIZE;
    }

    for (uint8_t i = POWER_HISTORY_SIZE - count; i < POWER_HISTORY_SIZE; i++)
    {
        if (powerHistory[i] > 0)
        {
            sum += powerHistory[i];
            validCount++;
        }
    }
    return sum / validCount;
}

float getPower()
{
    return powerHistory[POWER_HISTORY_SIZE - 1];
}