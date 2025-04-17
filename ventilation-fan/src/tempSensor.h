#pragma once

#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#define DHT_PIN 27
#define DHT_TYPE DHT11
#define TEMP_HISTORY_SIZE 60    // 5 minutes
#define TEMP_CURRENT_AVG_SIZE 4 // Last 20 seconds
#define TEMP_POLL_TIMER 2000    // Poll every 5 seconds
DHT dht(DHT_PIN, DHT_TYPE);

void initTempSensor()
{
    dht.begin();
    Serial.println("🌡️ Sensor de temperatura iniciado.");
}

float tempHistory[TEMP_HISTORY_SIZE];
void updateTempHistory(float t)
{
    for (uint8_t i = 0; i < TEMP_HISTORY_SIZE - 1; i++)
    {
        tempHistory[i] = tempHistory[i + 1];
    }
    tempHistory[TEMP_HISTORY_SIZE - 1] = t;
}

uint32_t lastTempTimeMeasurement = 0;
void pollTempSensor()
{
    if (millis() - lastTempTimeMeasurement > TEMP_POLL_TIMER)
    {
        lastTempTimeMeasurement = millis();
        // float t = dht.readTemperature();
        float t = temperatureRead();
        Serial.print("Valor de temperatura leido: ");
        Serial.println(t);
        if (!isnan(t))
        {
            updateTempHistory(t);
        }
    }
}

float getAverageTemp()
{
    float sum = 0;
    uint8_t validCount = 0;
    for (uint8_t i = 0; i < TEMP_HISTORY_SIZE; i++)
    {
        if (tempHistory[i] > 0)
        {
            sum += tempHistory[i];
            validCount++;
        }
    }
    return sum / validCount;
}

float getTemp()
{
    float sum = 0;
    uint8_t validCount = 0;

    for (uint8_t i = TEMP_HISTORY_SIZE - TEMP_CURRENT_AVG_SIZE; i < TEMP_HISTORY_SIZE; i++)
    {
        if (tempHistory[i] > 0)
        {
            sum += tempHistory[i];
            validCount++;
        }
    }
    return sum / validCount;
}

float getRawTemp()
{
    return tempHistory[TEMP_HISTORY_SIZE - 1];
}