#pragma once

#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <tempHistory.h>

#define DHT_PIN 27
#define DHT_TYPE DHT11
#define TEMP_HISTORY_SIZE 60    // 5 minutes
#define TEMP_CURRENT_AVG_SIZE 4 // Last 20 seconds
#define TEMP_POLL_TIMER 60_000    // Poll every minute
DHT dht(DHT_PIN, DHT_TYPE);

void initTempSensor()
{
    dht.begin();
    Serial.println("🌡️ Sensor de temperatura iniciado.");
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
            addTempToHistory(t);
        }
    }
}