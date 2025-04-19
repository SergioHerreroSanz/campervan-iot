#pragma once

#include <NimBLEDevice.h>
#include <fan.h>

class BLETargetTempCallbacks : public NimBLECharacteristicCallbacks
{
    virtual void onRead(NimBLECharacteristic *pCharacteristic, NimBLEConnInfo &connInfo)
    {
        float targetTemp = getUserTempTarget();
        pCharacteristic->setValue((uint8_t *)&targetTemp, sizeof(targetTemp));
    }
    void onWrite(NimBLECharacteristic *pCharacteristic, NimBLEConnInfo &connInfo) override
    {
        std::string value = pCharacteristic->getValue();

        if (value.length() == sizeof(float))
        {
            float targetTempValue;
            memcpy(&targetTempValue, value.data(), sizeof(float));
            Serial.print("Valor temperatura recibido: ");
            Serial.println(targetTempValue);
            setUserTempTarget(targetTempValue);
            pCharacteristic->notify();
        }
        else
        {
            Serial.println("Error: No se recibió valor temperatura válido.");
        }
    }
};