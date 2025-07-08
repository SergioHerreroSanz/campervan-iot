#pragma once

#include <NimBLEDevice.h>

class MyBLEServerCallbacks : public NimBLEServerCallbacks
{
    void onConnect(NimBLEServer *pServer, NimBLEConnInfo &connInfo) override
    {
        Serial.println("💡 Dispositivo BLE conectado.");
        NimBLEDevice::startAdvertising();
    }

    void onDisconnect(NimBLEServer *pServer, NimBLEConnInfo &connInfo, int reason) override
    {
        Serial.println("🔌 Dispositivo BLE desconectado.");
        NimBLEDevice::startAdvertising();
    }
};