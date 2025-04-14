#pragma once

#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#define DHTPIN 27
#define DHTTYPE DHT11
#define TEMP_HISTORY_SIZE 60 // 5 minutes
#define TEMP_CURRENT_AVG_SIZE 4 // Last 20 seconds
#define TEMP_POLL_TIMER 5000 // Poll every 5 seconds
DHT dht(DHTPIN, DHTTYPE);

void initTempSensor()
{
    dht.begin();
    Serial.println("🌡️ Sensor de temperatura iniciado.");
}

uint32_t lastTimeMeasurement = 0;
void pollTempSensor()
{
    if (millis() - lastTimeMeasurement > TEMP_POLL_TIMER){
        lastTimeMeasurement = millis();
        float t = dht.readTemperature();
        if(!isnan(t)){
            updateTempHistory(t);
        }
    }
}

float tempHistory[TEMP_HISTORY_SIZE];
void updateTempHistory(float t) {
    for (uint8_t i = 0; i < TEMP_HISTORY_SIZE - 1; i++) {
        tempHistory[i] = tempHistory[i + 1];
    }
    tempHistory[TEMP_HISTORY_SIZE - 1] = t;
}

float getTempAverage() {
    float sum = 0;
    uint8_t validCount = 0
    for (uint8_t i = 0; i < TEMP_HISTORY_SIZE; i++) {
        if (tempHistory[i] > 0) { 
            sum += tempHistory[i];
            validCount++;
        }
    return sum / validCount;
}

float getTemp() {
    float sum = 0;
    uint8_t validCount = 0

    for (uint8_t i = TEMP_HISTORY_SIZE - TEMP_CURRENT_AVG_SIZE; i < TEMP_HISTORY_SIZE; i++) {
        if (tempHistory[i] > 0) { 
            sum += tempHistory[i];
            validCount++;
        }    
    }
    return sum / validCount;
}

float getLastTempValue() {
    return tempHistory[TEMP_HISTORY_SIZE - 1];
}