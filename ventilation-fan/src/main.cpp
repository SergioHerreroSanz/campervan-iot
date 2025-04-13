#include <Arduino.h>
#include <NimBLEDevice.h>

#include <ble.h>
#include <fan.h>
#include <tempSensor.h>

void setup()
{
  Serial.begin(115200);
  delay(1000);

  initFan();
  initBLE();
  initTempSensor();
}

void loop()
{
}