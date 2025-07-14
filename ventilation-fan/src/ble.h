#pragma once

#include <NimBLEDevice.h>
#include <bleClientCallbacks.h>
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
#define BLE_SEARCH_TIME 5000

NimBLECharacteristic *modeCharacteristic;
NimBLECharacteristic *manualPowerCharacteristic;
NimBLECharacteristic *targetTempCharacteristic;
NimBLECharacteristic *currentPowerCharacteristic;
NimBLECharacteristic *rawTempCharacteristic;
NimBLEClient *client;
NimBLERemoteCharacteristic *remoteChar;
boolean serverMode = false;

bool subscribeToCharacteristic(
    NimBLERemoteService *service,
    const char *uuid,
    size_t expectedSize,
    std::function<void(uint8_t *data)> onNotify)
{
    NimBLERemoteCharacteristic *c = service->getCharacteristic(uuid);
    if (!c)
    {
        Serial.print("❌ Característica no encontrada: ");
        Serial.println(uuid);
        return false;
    }

    if (!c->canNotify())
    {
        Serial.print("⚠️ La característica no permite notificaciones: ");
        Serial.println(uuid);
        return false;
    }

    bool ok = c->subscribe(true, [expectedSize, onNotify, uuid](NimBLERemoteCharacteristic *c, uint8_t *data, size_t len, bool isNotify)
                           {
                               if (len < expectedSize)
                               {
                                   Serial.print("❌ Notificación muy corta en ");
                                   Serial.println(uuid);
                                   return;
                               }

                               onNotify(data); });

    if (ok)
    {
        Serial.print("✅ Suscrito a ");
        Serial.println(uuid);
    }
    else
    {
        Serial.print("❌ Error al suscribirse a ");
        Serial.println(uuid);
    }

    return ok;
}

boolean conectServer()
{
    delay(100);
    NimBLEDevice::deinit(true);
    delay(100);
    NimBLEDevice::init("ESP32Client");
    NimBLEDevice::setPower(ESP_PWR_LVL_P9);
    NimBLEScan *scan = NimBLEDevice::getScan();

    scan->clearResults();
    scan->setActiveScan(true);
    scan->setInterval(45);
    scan->setWindow(15);
    scan->start(BLE_SEARCH_TIME, false);

    Serial.println("Escaneando dispositivos BLE...");
    delay(BLE_SEARCH_TIME + 1000);

    NimBLEScanResults results = scan->getResults();

    Serial.print(results.getCount());
    Serial.println(" dispositivos encontrados");

    for (int i = 0; i < results.getCount(); i++)
    {
        const NimBLEAdvertisedDevice *dev = results.getDevice(i);

        auto name = dev->getName();
        if (!name.empty() && strcmp(name.c_str(), BLE_DEVICE_NAME) == 0)
        {
            Serial.println("Servidor encontrado, intentando conectar...");

            client = NimBLEDevice::createClient();
            client->setClientCallbacks(new MyBLEClientCallbacks());
            if (!client->connect(dev))
            {
                Serial.println("❌ Error de conexión");
                NimBLEDevice::deleteClient(client);
                return false;
            }

            NimBLERemoteService *service = client->getService(BLE_SERVICE_ID);
            if (!service)
            {
                Serial.println("❌ Servicio no encontrado");
                client->disconnect();
                return false;
            }

            subscribeToCharacteristic(
                service,
                BLE_CURRENT_POWER_CHARACTERISTIC_ID,
                sizeof(float),
                [](uint8_t *data)
                {
                    float power;
                    memcpy(&power, data, sizeof(float));
                    Serial.print("🔧 Power recibido: ");
                    Serial.println(power, 2);
                    changePower(power);
                });

            subscribeToCharacteristic(
                service,
                BLE_MODE_CHARACTERISTIC_ID,
                sizeof(uint8_t),
                [](uint8_t *data)
                {
                    FanMode mode = static_cast<FanMode>(data[0]);
                    Serial.print("📥 Fan mode recibido: ");
                    Serial.println((int)mode);
                    setFanMode(mode);
                });

            subscribeToCharacteristic(
                service,
                BLE_MANUAL_POWER_CHARACTERISTIC_ID,
                sizeof(float),
                [](uint8_t *data)
                {
                    float manualPower;
                    memcpy(&manualPower, data, sizeof(float));
                    Serial.print("📥 Potencia manual recibida: ");
                    Serial.println(manualPower, 2);
                    setFanManualPowerTarget(manualPower);
                });

            subscribeToCharacteristic(
                service,
                BLE_TARGET_TEMP_CHARACTERISTIC_ID,
                sizeof(float),
                [](uint8_t *data)
                {
                    float targetTemp;
                    memcpy(&targetTemp, data, sizeof(float));
                    Serial.print("📥 Temperatura objetivo recibida: ");
                    Serial.println(targetTemp, 2);
                    setUserTempTarget(targetTemp);
                });

            return true;
        }
    }

    scan->clearResults();
    NimBLEDevice::deinit(true);
    Serial.println("No se encontró otro ESP32");

    return false;
}

void startServer()
{
    Serial.println("Iniciando servidor");

    NimBLEDevice::init(BLE_DEVICE_NAME);
    NimBLEAdvertising *pAdvertising = NimBLEDevice::getAdvertising();

    NimBLEAdvertisementData advData;
    advData.setName(BLE_DEVICE_NAME);
    advData.addServiceUUID(BLE_SERVICE_ID);
    pAdvertising->setAdvertisementData(advData);

    NimBLEAdvertisementData scanRespData;
    scanRespData.setShortName(BLE_DEVICE_NAME);
    pAdvertising->setScanResponseData(scanRespData);

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
    serverMode = true;
    Serial.println("Esperando conexión...");
}

void initBLE()
{
    if (!conectServer())
    {
        startServer();
    }
}

uint32_t lastNotifyMeasurement = 0;
void notifyTemps()
{
    if (serverMode && millis() - lastNotifyMeasurement > BLE_NOTIFY_FREQUENCY)
    {
        lastNotifyMeasurement = millis();

        float fanPower = getLastPower();
        currentPowerCharacteristic->setValue((uint8_t *)&fanPower, sizeof(fanPower));
        currentPowerCharacteristic->notify();

        float temp = getLastTemp();
        rawTempCharacteristic->setValue((uint8_t *)&temp, sizeof(temp));
        rawTempCharacteristic->notify();
    }
}
