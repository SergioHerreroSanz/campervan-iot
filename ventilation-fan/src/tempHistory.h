#pragma once

#define TEMP_HISTORY_SIZE 60
float tempHistory[TEMP_HISTORY_SIZE];

void addTempToHistory(float t)
{
    for (uint8_t i = 0; i < TEMP_HISTORY_SIZE - 1; i++)
    {
        tempHistory[i] = tempHistory[i + 1];
    }
    tempHistory[TEMP_HISTORY_SIZE - 1] = t;
}

float getAverageTemp(uint8_t count)
{
    float sum = 0;
    uint8_t validCount = 0;

    if (count > TEMP_HISTORY_SIZE)
    {
        count = TEMP_HISTORY_SIZE;
    }

    for (uint8_t i = TEMP_HISTORY_SIZE - count; i < TEMP_HISTORY_SIZE; i++)
    {
        if (tempHistory[i] > 0)
        {
            sum += tempHistory[i];
            validCount++;
        }
    }
    return sum / validCount;
}

float getTemp(uint8_t count)
{
    if (count > TEMP_HISTORY_SIZE)
    {
        count = TEMP_HISTORY_SIZE;
    }
    float value = tempHistory[TEMP_HISTORY_SIZE - count];

    return value > 0 ? value : getTemp(count - 1)
}

float getLastTemp()
{
    return tempHistory[TEMP_HISTORY_SIZE - 1];
}
