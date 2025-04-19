#pragma once

#include <NimBLEDevice.h>
#include <fan.h>

class BLEManualPowerCallbacks : public NimBLECharacteristicCallbacks
{
    virtual void onRead(NimBLECharacteristic *pCharacteristic, NimBLEConnInfo &connInfo)
    {
        float power = getFanManualPowerTarget();
        pCharacteristic->setValue((uint8_t *)&power, sizeof(power));
    }
    void onWrite(NimBLECharacteristic *pCharacteristic, NimBLEConnInfo &connInfo) override
    {
        std::string value = pCharacteristic->getValue();

        if (value.length() == sizeof(float))
        {
            float pwmValue;
            memcpy(&pwmValue, value.data(), sizeof(float));
            Serial.print("Valor potencia recibido: ");
            Serial.println(pwmValue);
            if (pwmValue > 0 && pwmValue < 1)
            {
                setFanManualPowerTarget(pwmValue);
                pCharacteristic->notify();
            }
            else
            {
                Serial.println("Error: Valor potencia fuera de rango");
            }
        }
        else
        {
            Serial.println("Error: No se recibió valor potencia válido.");
        }
    }
};