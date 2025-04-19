#pragma once

#include <NimBLEDevice.h>
#include <fan.h>

class BLEModeCallbacks : public NimBLECharacteristicCallbacks
{
    virtual void onRead(NimBLECharacteristic *pCharacteristic, NimBLEConnInfo &connInfo)
    {
        pCharacteristic->setValue(getFanMode());
    }
    void onWrite(NimBLECharacteristic *pCharacteristic, NimBLEConnInfo &connInfo) override
    {
        std::string value = pCharacteristic->getValue();
        if (!value.empty())
        {
            uint8_t modeValue = (uint8_t)value[0];
            if (modeValue == 0)
            {
                Serial.println("Modo APAGADO seleccionado desde BLE.");
                setFanMode(FAN_MODE_OFF);
            }
            else if (modeValue == 1)
            {
                Serial.println("Modo MANUAL seleccionado desde BLE.");
                setFanMode(FAN_MODE_MANUAL);
            }
            else if (modeValue == 2)
            {
                Serial.println("Modo TURBO seleccionado desde BLE.");
                setFanMode(FAN_MODE_TURBO);
            }
            else if (modeValue == 3)
            {
                Serial.println("Modo AUTO LOW seleccionado desde BLE.");
                setFanMode(FAN_MODE_AUTO_LOW);
            }
            else if (modeValue == 4)
            {
                Serial.println("Modo AUTO HIGH seleccionado desde BLE.");
                setFanMode(FAN_MODE_AUTO_HIGH);
            }
            else if (modeValue == 5)
            {
                Serial.println("Modo SILENT seleccionado desde BLE.");
                setFanMode(FAN_MODE_SILENT);
            }
            pCharacteristic->notify();
        }
    }
};