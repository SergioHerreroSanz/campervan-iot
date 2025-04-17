#pragma once

#include <NimBLEDevice.h>
#include <tempSensor.h>

class BLEIntTempCallbacks : public NimBLECharacteristicCallbacks
{
    virtual void onRead(NimBLECharacteristic *pCharacteristic, NimBLEConnInfo &connInfo)
    {
        float intTemp = getTemp();
        pCharacteristic->setValue((uint8_t *)&intTemp, sizeof(intTemp));
    }
};