#include <Arduino.h>
#include <NimBLEDevice.h>

// Definir el pin del LED (usando PWM)
#define LED_PIN 13

// Definir la característica BLE
NimBLECharacteristic *pwmCharacteristic;

// Crear una clase derivada de NimBLECharacteristicCallbacks para gestionar el callback
class MyCallbacks : public NimBLECharacteristicCallbacks
{
  void onWrite(NimBLECharacteristic *pCharacteristic, NimBLEConnInfo &connInfo) override
  {
    // Log para confirmar que se ha llamado a onWrite
    Serial.println("onWrite llamado");

    // Obtener el valor que se ha escrito en la característica
    std::string value = pCharacteristic->getValue();

    // Verificar que el valor recibido no esté vacío
    if (value.length() > 0)
    {
      // Obtener el primer byte del valor
      uint8_t pwmValue = value[0];

      // Asegurarse de que el valor PWM esté en el rango de 0-255
      if (pwmValue >= 0 && pwmValue <= 255)
      {
        Serial.print("Valor PWM recibido: ");
        Serial.println(pwmValue);

        // Ajustar el valor PWM para controlar el brillo del LED
        ledcWrite(0, pwmValue); // Canal 0 para controlar el LED en GPIO 13
      }
      else
      {
        Serial.println("Error: Valor fuera de rango (0-255)");
      }
    }
    else
    {
      Serial.println("Error: No se recibió valor válido.");
    }
  }
};

// Configuración inicial en el setup
void setup()
{
  Serial.begin(115200);
  delay(1000); // Tiempo para permitir que el monitor serial se inicie

  // Configurar el pin del LED como salida PWM
  pinMode(LED_PIN, OUTPUT);

  // Configuración de PWM en el pin 13
  ledcSetup(0, 5000, 8);     // Canal 0, 5 kHz, 8 bits de resolución
  ledcAttachPin(LED_PIN, 0); // Asocia el pin al canal PWM

  // Inicializar BLE
  NimBLEDevice::init("ESP32");
  NimBLEDevice::setDeviceName("ESP32");

  // Crear el servidor BLE
  NimBLEServer *pServer = NimBLEDevice::createServer();

  // Crear un servicio BLE
  NimBLEService *pService = pServer->createService("1234"); // UUID del servicio

  // Crear una característica BLE para controlar el PWM
  pwmCharacteristic = pService->createCharacteristic(
      "5678",                                        // UUID de la característica
      NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE // Hacerla tanto de lectura como escritura
  );

  // Establecer el callback para la escritura en la característica
  pwmCharacteristic->setCallbacks(new MyCallbacks());

  // Establecer el valor inicial de la característica (por ejemplo, 0)
  pwmCharacteristic->setValue(0);

  // Iniciar el servicio BLE
  pService->start();

  // Create an advertising object and add the service UUID
  NimBLEAdvertising *pAdvertising = NimBLEDevice::getAdvertising();
  NimBLEAdvertisementData responseData;
  responseData.setName("ESP32_BLE_Device");
  responseData.setShortName("ESP32");
  responseData.setManufacturerData("Yogurin");
  pAdvertising->setScanResponseData(responseData);

  // Iniciar la publicidad BLE
  pAdvertising->start();

  Serial.println("Esperando conexión...");
}

void loop()
{
  // El loop está vacío, ya que el manejo se hace en el callback
  // Aquí puedes agregar otras funcionalidades si lo deseas
}