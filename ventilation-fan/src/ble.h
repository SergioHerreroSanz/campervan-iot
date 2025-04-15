#pragma once

#include <NimBLEDevice.h>
#include <bleServerCallbacks.h>
#include <bleModeCallbacks.h>
#include <blePowerCallbacks.h>
#include <bleTargetTempCallbacks.h>
#include <bleIntTempCallbacks.h>
#include <bleExtTempCallbacks.h>
#include <fan.h>

#define BLE_DEVICE_NAME "ESP32-ventiladores"
#define BLE_SERVICE_ID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define BLE_POWER_CHARACTERISTIC_ID "beb5483e-36e1-4688-b7f5-ea07361b26a8"
#define BLE_MODE_CHARACTERISTIC_ID "6aee789a-4bcd-4c90-a8ef-456dfe253ad1"
#define BLE_TARGET_TEMP_CHARACTERISTIC_ID "6aee789a-4bcd-4c90-a8ef-456dfe253ad2"
#define BLE_INT_TEMP_CHARACTERISTIC_ID "6aee789a-4bcd-4c90-a8ef-456dfe253ad3"
#define BLE_EXT_TEMP_CHARACTERISTIC_ID "6aee789a-4bcd-4c90-a8ef-456dfe253ad4"

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
    pwmCharacteristic->addDescriptor(new BLE2902());
    pwmCharacteristic->setNotifyProperty(true);

    modeCharacteristic = pService->createCharacteristic(
        BLE_MODE_CHARACTERISTIC_ID,
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::NOTIFY);
    modeCharacteristic->setCallbacks(new BLEModeCallbacks());
    modeCharacteristic->addDescriptor(new BLE2902());
    modeCharacteristic->setNotifyProperty(true);

    targetTempCharacteristic = pService->createCharacteristic(
        BLE_TARGET_TEMP_CHARACTERISTIC_ID,
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::NOTIFY);
    targetTempCharacteristic->setCallbacks(new BLETargetTempCallbacks());
    targetTempCharacteristic->addDescriptor(new BLE2902());
    targetTempCharacteristic->setNotifyProperty(true);

    intTempCharacteristic = pService->createCharacteristic(
        BLE_INT_TEMP_CHARACTERISTIC_ID,
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY);
    intTempCharacteristic->setCallbacks(new BLEIntTempCallbacks());
    intTempCharacteristic->addDescriptor(new BLE2902());
    intTempCharacteristic->setNotifyProperty(true);

    extTempCharacteristic = pService->createCharacteristic(
        BLE_EXT_TEMP_CHARACTERISTIC_ID,
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY);
    extTempCharacteristic->setCallbacks(new BLEExtTempCallbacks());
    extTempCharacteristic->addDescriptor(new BLE2902());
    extTempCharacteristic->setNotifyProperty(true);
    
    rawTempCharacteristic = pService->createCharacteristic(
        BLE_EXT_TEMP_CHARACTERISTIC_ID,
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY);
    rawTempCharacteristic->setCallbacks(new BLERawTempCallbacks());
    rawTempCharacteristic->addDescriptor(new BLE2902());
    rawTempCharacteristic->setNotifyProperty(true);

    pService->start();
    pAdvertising->start();
    Serial.println("Esperando conexión...");
}

void notifyTemps()
{
    intTempCharacteristic->nofify();
    extTempCharacteristic->nofify();
    rawTempCharacteristic->nofify();
}