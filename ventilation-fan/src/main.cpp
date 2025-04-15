#include <Arduino.h>
#include <ble.h>
#include <fan.h>
#include <tempSensor.h>

void setup()
{
  Serial.begin(115200);
  delay(1000);

  initTempSensor();
  initFan();
  initBLE();
}

void loop()
{
  pollTempSensor();
  runFan();
}