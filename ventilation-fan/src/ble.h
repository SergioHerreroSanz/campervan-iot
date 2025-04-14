#pragma once

#include <NimBLEDevice.h>
#include <bleServerCallbacks.h>
#include <bleModeCallbacks.h>
#include <blePowerCallbacks.h>
#include <fan.h>

#define BLE_DEVICE_NAME "ESP32-ventiladores"
#define BLE_SERVICE_ID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define BLE_POWER_CHARACTERISTIC_ID "beb5483e-36e1-4688-b7f5-ea07361b26a8"
#define BLE_MODE_CHARACTERISTIC_ID "6aee789a-4bcd-4c90-a8ef-456dfe253ad1"

NimBLECharacteristic *pwmCharacteristic;
NimBLECharacteristic *modeCharacteristic;

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
    uint8_t pwmInitVal[2] = {
        (uint8_t)(initialPWM & 0xFF),       // lower byte
        (uint8_t)((initialPWM >> 8) & 0xFF) // upper byte
    };
    pwmCharacteristic->setValue(pwmInitVal, 2);

    modeCharacteristic = pService->createCharacteristic(
        BLE_MODE_CHARACTERISTIC_ID,
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE);
    modeCharacteristic->setCallbacks(new BLEModeCallbacks());
    uint8_t modeInitVal = 0;
    modeCharacteristic->setValue(&modeInitVal, 1);

    pService->start();
    pAdvertising->start();
    Serial.println("Esperando conexión...");
}