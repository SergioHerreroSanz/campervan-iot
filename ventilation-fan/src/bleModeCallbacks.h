#pragma once

#include <NimBLEDevice.h>
#include <fan.h>

class BLEModeCallbacks : public NimBLECharacteristicCallbacks
{
    void onWrite(NimBLECharacteristic *pCharacteristic, NimBLEConnInfo &connInfo) override
    {
        std::string value = pCharacteristic->getValue();
        if (!value.empty())
        {
            uint8_t modeValue = (uint8_t)value[0];
            if (modeValue == 0)
            {
                Serial.println("Modo MANUAL seleccionado desde BLE.");
                // setFanMode(FAN_MODE_MANUAL);
            }
            else if (modeValue == 1)
            {
                Serial.println("Modo AUTOMÁTICO seleccionado desde BLE.");
                // setFanMode(FAN_MODE_AUTO);
            }
            else
            {
                Serial.println("⚠️ Valor inválido para el modo.");
            }
        }
    }
};