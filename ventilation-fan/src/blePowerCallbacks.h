#pragma once

#include <NimBLEDevice.h>
#include <Arduino.h>
#include <fan.h>

class BLEPowerCallbacks : public NimBLECharacteristicCallbacks
{
    virtual void onRead(NimBLECharacteristic *pCharacteristic, NimBLEConnInfo &connInfo)
    {
        pCharacteristic->setValue(getFanManualPowerTarget());
    }
    void onWrite(NimBLECharacteristic *pCharacteristic, NimBLEConnInfo &connInfo) override
    {
        std::string value = pCharacteristic->getValue();

        if (value.length() >= (PWM_RESOLUTION / 8))
        {
            uint16_t pwmValue = (uint8_t)value[0] | ((value.size() > 1 ? (uint8_t)value[1] : 0) << 8);

            if (pwmValue <= (pow(2, PWM_RESOLUTION)))
            {
                Serial.print("Valor PWM recibido: ");
                Serial.println(pwmValue);
                setFanManualPowerTarget(pwmValue);
            }
            else
            {
                Serial.println("Error: Valor PWM fuera de rango");
            }
        }
        else
        {
            Serial.println("Error: No se recibió valor PWM válido.");
        }
    }
};