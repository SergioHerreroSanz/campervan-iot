#pragma once

#include <NimBLEDevice.h>

class MyBLEClientCallbacks : public NimBLEClientCallbacks
{
    void onConnect(NimBLEClient *pClient) override
    {
        Serial.println("💡Conectado al servidor BLE");
    }

    void onDisconnect(NimBLEClient *pClient, int reason) override
    {
        Serial.println("🔌Desconectado del servidor BLE.");
        Serial.println("Reiniciando...");
        ESP.restart();
    }
};