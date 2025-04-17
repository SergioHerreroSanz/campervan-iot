#pragma once

#include <NimBLEDevice.h>
#include <tempSensor.h>

class BLERawTempCallbacks : public NimBLECharacteristicCallbacks
{
    virtual void onRead(NimBLECharacteristic *pCharacteristic, NimBLEConnInfo &connInfo)
    {
        float rawTemp = getRawTemp();
        pCharacteristic->setValue((uint8_t *)&rawTemp, sizeof(rawTemp));
    }
};