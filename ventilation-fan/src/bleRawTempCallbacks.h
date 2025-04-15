#pragma once

#include <NimBLEDevice.h>
#include <tempSensor.h>

class BLERawTempCallbacks : public NimBLECharacteristicCallbacks
{
    virtual void onRead(NimBLECharacteristic *pCharacteristic, NimBLEConnInfo &connInfo)
    {
        pCharacteristic->setValue(getRawTemp());
    }
};