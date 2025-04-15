#pragma once

#include <NimBLEDevice.h>
#include <tempSensor.h>

class BLEIntTempCallbacks : public NimBLECharacteristicCallbacks
{
    virtual void onRead(NimBLECharacteristic *pCharacteristic, NimBLEConnInfo &connInfo)
    {
        pCharacteristic->setValue(getTempAverage());
    }
};