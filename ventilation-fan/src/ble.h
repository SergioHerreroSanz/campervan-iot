#pragma once

#include <NimBLEDevice.h>
#include <bleServerCallbacks.h>
#include <bleModeCallbacks.h>
#include <blePowerCallbacks.h>
#include <bleTargetTempCallbacks.h>
#include <bleIntTempCallbacks.h>
#include <bleExtTempCallbacks.h>
#include <bleRawTempCallbacks.h>
#include <fan.h>

#define BLE_DEVICE_NAME "ESP32-ventiladores"
#define BLE_SERVICE_ID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define BLE_POWER_CHARACTERISTIC_ID "beb5483e-36e1-4688-b7f5-ea07361b26a8"
#define BLE_MODE_CHARACTERISTIC_ID "6aee789a-4bcd-4c90-a8ef-456dfe253ad1"
#define BLE_TARGET_TEMP_CHARACTERISTIC_ID "6aee789a-4bcd-4c90-a8ef-456dfe253ad2"
#define BLE_INT_TEMP_CHARACTERISTIC_ID "6aee789a-4bcd-4c90-a8ef-456dfe253ad3"
#define BLE_EXT_TEMP_CHARACTERISTIC_ID "6aee789a-4bcd-4c90-a8ef-456dfe253ad4"
#define BLE_RAW_TEMP_CHARACTERISTIC_ID "6aee789a-4bcd-4c90-a8ef-456dfe253ad5"
#define BLE_NOTIFY_FREQUENCY 1000

NimBLECharacteristic *pwmCharacteristic;
NimBLECharacteristic *modeCharacteristic;
NimBLECharacteristic *targetTempCharacteristic;
NimBLECharacteristic *intTempCharacteristic;
NimBLECharacteristic *extTempCharacteristic;
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

    pwmCharacteristic = pService->createCharacteristic(
        BLE_POWER_CHARACTERISTIC_ID,
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::NOTIFY);
    pwmCharacteristic->setCallbacks(new BLEPowerCallbacks());

    modeCharacteristic = pService->createCharacteristic(
        BLE_MODE_CHARACTERISTIC_ID,
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::NOTIFY);
    modeCharacteristic->setCallbacks(new BLEModeCallbacks());

    targetTempCharacteristic = pService->createCharacteristic(
        BLE_TARGET_TEMP_CHARACTERISTIC_ID,
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::NOTIFY);
    targetTempCharacteristic->setCallbacks(new BLETargetTempCallbacks());

    intTempCharacteristic = pService->createCharacteristic(
        BLE_INT_TEMP_CHARACTERISTIC_ID,
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY);
    intTempCharacteristic->setCallbacks(new BLEIntTempCallbacks());

    extTempCharacteristic = pService->createCharacteristic(
        BLE_EXT_TEMP_CHARACTERISTIC_ID,
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY);
    extTempCharacteristic->setCallbacks(new BLEExtTempCallbacks());

    rawTempCharacteristic = pService->createCharacteristic(
        BLE_RAW_TEMP_CHARACTERISTIC_ID,
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

        float intTemp = getRawTemp();
        intTempCharacteristic->setValue((uint8_t *)&intTemp, sizeof(intTemp));
        intTempCharacteristic->notify();

        float extTemp = getRawTemp();
        extTempCharacteristic->setValue((uint8_t *)&extTemp, sizeof(extTemp));
        extTempCharacteristic->notify();

        float rawTemp = getRawTemp();
        rawTempCharacteristic->setValue((uint8_t *)&rawTemp, sizeof(rawTemp));
        rawTempCharacteristic->notify();
    }
}