#pragma once

#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

// Cambia este pin si tu sensor está conectado a otro
#define DHTPIN 27
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

void initTempSensor()
{
    dht.begin();
    Serial.println("🌡️ Sensor de temperatura iniciado.");
}

float readTemperature()
{
    float t = dht.readTemperature();
    return isnan(t) ? -1 : t;
}