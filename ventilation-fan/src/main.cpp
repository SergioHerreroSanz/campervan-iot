#include <Arduino.h>
#include <NimBLEDevice.h>

#include <ble.h>
#include <fan.h>

void setup()
{
  //Serial.begin(115200);
  //delay(1000);

  initFan();
  initBLE();
}

void loop()
{
}