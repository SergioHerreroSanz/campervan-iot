#pragma once

#include <NimBLEDevice.h>
#include <bleServerCallbacks.h>
#include <bleModeCallbacks.h>
#include <bleManualPowerCallbacks.h>
#include <bleTargetTempCallbacks.h>
#include <bleCurrentPowerCallbacks.h>
#include <bleRawTempCallbacks.h>
#include <fan.h>

#define BLE_DEVICE_NAME "ESP32-ventiladores"
#define BLE_SERVICE_ID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define BLE_MODE_CHARACTERISTIC_ID "6aee789a-4bcd-4c90-a8ef-456dfe253ad1"
#define BLE_MANUAL_POWER_CHARACTERISTIC_ID "beb5483e-36e1-4688-b7f5-ea07361b26a8"
#define BLE_TARGET_TEMP_CHARACTERISTIC_ID "6aee789a-4bcd-4c90-a8ef-456dfe253ad2"
#define BLE_CURRENT_POWER_CHARACTERISTIC_ID "6aee789a-4bcd-4c90-a8ef-456dfe253ad3"
#define BLE_TEMP_CHARACTERISTIC_ID "6aee789a-4bcd-4c90-a8ef-456dfe253ad5"
#define BLE_NOTIFY_FREQUENCY 1000

NimBLECharacteristic *modeCharacteristic;
NimBLECharacteristic *manualPowerCharacteristic;
NimBLECharacteristic *targetTempCharacteristic;

NimBLECharacteristic *currentPowerCharacteristic;
NimBLECharacteristic *rawTempCharacteristic;

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

    modeCharacteristic = pService->createCharacteristic(
        BLE_MODE_CHARACTERISTIC_ID,
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::NOTIFY);
    modeCharacteristic->setCallbacks(new BLEModeCallbacks());

    manualPowerCharacteristic = pService->createCharacteristic(
        BLE_MANUAL_POWER_CHARACTERISTIC_ID,
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::NOTIFY);
    manualPowerCharacteristic->setCallbacks(new BLEManualPowerCallbacks());

    targetTempCharacteristic = pService->createCharacteristic(
        BLE_TARGET_TEMP_CHARACTERISTIC_ID,
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::NOTIFY);
    targetTempCharacteristic->setCallbacks(new BLETargetTempCallbacks());

    currentPowerCharacteristic = pService->createCharacteristic(
        BLE_CURRENT_POWER_CHARACTERISTIC_ID,
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY);
    currentPowerCharacteristic->setCallbacks(new BLECurrentPowerCallbacks());

    rawTempCharacteristic = pService->createCharacteristic(
        BLE_TEMP_CHARACTERISTIC_ID,
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY);
    rawTempCharacteristic->setCallbacks(new BLERawTempCallbacks());

    pService->start();
    pAdvertising->start();
    Serial.println("Esperando conexión...");
}

uint32_t lastNotifyMeasurement = 0;
void notifyTemps()
{
    if (millis() - lastNotifyMeasurement > BLE_NOTIFY_FREQUENCY)
    {
        lastNotifyMeasurement = millis();

        float fanPower = getPower();
        currentPowerCharacteristic->setValue((uint8_t *)&fanPower, sizeof(fanPower));
        currentPowerCharacteristic->notify();

        float temp = getTemp();
        rawTempCharacteristic->setValue((uint8_t *)&temp, sizeof(temp));
        rawTempCharacteristic->notify();
    }
}