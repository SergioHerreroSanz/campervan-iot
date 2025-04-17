#pragma once

#include <NimBLEDevice.h>
#include <tempSensor.h>

class BLEExtTempCallbacks : public NimBLECharacteristicCallbacks
{
    virtual void onRead(NimBLECharacteristic *pCharacteristic, NimBLEConnInfo &connInfo)
    {
        float extTemp = getAverageTemp();
        pCharacteristic->setValue((uint8_t *)&extTemp, sizeof(extTemp));
    }
};