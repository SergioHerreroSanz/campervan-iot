#pragma once

#include <NimBLEDevice.h>
#include <powerHistory.h>

class BLECurrentPowerCallbacks : public NimBLECharacteristicCallbacks
{
    virtual void onRead(NimBLECharacteristic *pCharacteristic, NimBLEConnInfo &connInfo)
    {
        float power = getLastPower();
        pCharacteristic->setValue((uint8_t *)&power, sizeof(power));
    }
};