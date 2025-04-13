#pragma once

#include <NimBLEDevice.h>
#include <fan.h>

// Definir la configuración de ble
constexpr const char *BLE_DEVICE_NAME = "ESP32-ventiladores";
constexpr const char *BLE_SERVICE_ID = "4fafc201-1fb5-459e-8fcc-c5c9c331914b";
constexpr const char *BLE_POWER_CHARACTERISTIC_ID = "beb5483e-36e1-4688-b7f5-ea07361b26a8";

NimBLECharacteristic *pwmCharacteristic;

class MyBLEServerCallbacks : public NimBLEServerCallbacks
{
    void onConnect(NimBLEServer *pServer, NimBLEConnInfo &connInfo) override
    {
        Serial.println("💡 Dispositivo BLE conectado.");
    }

    void onDisconnect(NimBLEServer* pServer, NimBLEConnInfo& connInfo, int reason) override
    {
        Serial.println("🔌 Dispositivo BLE desconectado.");
        NimBLEDevice::startAdvertising(); // También puedes usar pServer->getAdvertising()->start();
    }
};

class BLEPowerCallbacks : public NimBLECharacteristicCallbacks
{
    void onWrite(NimBLECharacteristic *pCharacteristic, NimBLEConnInfo &connInfo) override
    {
        std::string value = pCharacteristic->getValue();

        if (value.length() >= (PWM_RESOLUTION / 8)) // Comprobar que tenemos los bytes necesarios
        {
            uint16_t pwmValue = (uint8_t)value[0] | ((value.size() > 1 ? (uint8_t)value[1] : 0) << 8);
            Serial.println(pwmValue);
            Serial.println(pow(2, PWM_RESOLUTION));

            if (pwmValue <= (pow(2, PWM_RESOLUTION)))
            {
                Serial.print("Valor PWM recibido: ");
                Serial.println(pwmValue);
                gradualPWMChange(pwmValue);
            }
            else
            {
                Serial.println("Error: Valor fuera de rango");
            }
        }
        else
        {
            Serial.println("Error: No se recibió valor válido.");
        }
    }
};

void initBLE()
{
    NimBLEDevice::init(BLE_DEVICE_NAME);

    NimBLEAdvertising *pAdvertising = NimBLEDevice::getAdvertising();
    NimBLEAdvertisementData responseData;
    responseData.setName(BLE_DEVICE_NAME);
    responseData.setShortName(BLE_DEVICE_NAME);
    pAdvertising->setScanResponseData(responseData);

    NimBLEServer *pServer = NimBLEDevice::createServer();
    pServer->setCallbacks(new MyBLEServerCallbacks());
    NimBLEService *pService = pServer->createService(BLE_SERVICE_ID);
    pwmCharacteristic = pService->createCharacteristic(
        BLE_POWER_CHARACTERISTIC_ID,
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE);
    pwmCharacteristic->setCallbacks(new BLEPowerCallbacks());

    uint16_t initialPWM = 1 << (PWM_RESOLUTION - 1);
    uint8_t initVal[2] = {
        (uint8_t)(initialPWM & 0xFF),       // lower byte
        (uint8_t)((initialPWM >> 8) & 0xFF) // upper byte
    };
    pwmCharacteristic->setValue(initVal, 2);

    pService->start();
    pAdvertising->start();
    Serial.println("Esperando conexión...");
}