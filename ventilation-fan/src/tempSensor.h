#pragma once

#include <Wire.h>
#include <Adafruit_SHT31.h>
#include <tempHistory.h>

#define SDA_PIN 22
#define SCL_PIN 21
#define TEMP_HISTORY_SIZE 60    // 5 minutes
#define TEMP_CURRENT_AVG_SIZE 4 // Last 20 seconds
#define TEMP_POLL_TIMER 60000   // Poll every minute
Adafruit_SHT31 sht31 = Adafruit_SHT31();

void initTempSensor()
{
    Wire.begin(SDA_PIN, SCL_PIN);

    if (!sht31.begin(0x44))
    {
        Serial.println("❌ Error al iniciar el sensor SHT31.");
    }
    else
    {
        Serial.println("🌡️ Sensor de temperatura iniciado.");
    }
}

uint32_t lastTempTimeMeasurement = 0;
void pollTempSensor()
{
    if (millis() - lastTempTimeMeasurement > TEMP_POLL_TIMER)
    {
        lastTempTimeMeasurement = millis();
        float t = sht31.readTemperature();
        Serial.print("Valor de temperatura leído: ");
        Serial.println(t);
        if (!isnan(t))
        {
            addTempToHistory(t);
        }
        else
        {
            Serial.println("⚠️ Error al leer la temperatura (NaN).");
        }
    }
}