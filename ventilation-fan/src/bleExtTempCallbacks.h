#pragma once

#include <NimBLEDevice.h>
#include <tempSensor.h>

class BLEExtTempCallbacks : public NimBLECharacteristicCallbacks
{
    virtual void onRead(NimBLECharacteristic *pCharacteristic, NimBLEConnInfo &connInfo)
    {
        pCharacteristic->setValue(getTemp());
    }
};