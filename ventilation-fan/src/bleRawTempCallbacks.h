#pragma once

#include <NimBLEDevice.h>
#include <tempSensor.h>

class BLERawTempCallbacks : public NimBLECharacteristicCallbacks
{
    virtual void onRead(NimBLECharacteristic *pCharacteristic, NimBLEConnInfo &connInfo)
    {
        float temp = getLastTemp();
        pCharacteristic->setValue((uint8_t *)&temp, sizeof(temp));
    }
};