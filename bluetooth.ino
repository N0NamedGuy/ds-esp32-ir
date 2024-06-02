#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>

#include "bluetooth.h"
#include "infrared.h"

#define SERVICE_UUID "4aaab213-83a8-4237-ac28-bfe0572a6d71"
#define SEND_CHARACTERISTIC_UUID "71929f8b-43e1-4f36-ab1f-25be58bfecea"

BLEServer *pServer;
BLEService *pService;
BLECharacteristic *pSendCharacteristic;

class ServerCallbacks: public BLEServerCallbacks {
  void onConnect(BLEServer* server) {
    Serial.println("Got connection");
  }
};

class SendIrCharacteristicCallbacks: public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic, esp_ble_gatts_cb_param_t* param) {
    const String value = String(pCharacteristic->getValue().c_str());

    Serial.println("We got a value, somehow");

    if (value.length() > 0) {
      Serial.printf("Got value %s\n", value.c_str());
    }
    if (value.startsWith("IRSEND POWER")) {
      Serial.printf("Sending IR power command\n");
      sendOn();
    }
  }
};

void initBluetooth() {
  Serial.println("start init bluetooth");
  BLEDevice::init("David's BlueRemote");
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new ServerCallbacks());
  pService = pServer->createService(SERVICE_UUID);
  pSendCharacteristic = pService->createCharacteristic(
    SEND_CHARACTERISTIC_UUID,
    BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE
  );
  pSendCharacteristic->setValue("Hello world");

  pSendCharacteristic->setCallbacks(new SendIrCharacteristicCallbacks());

  pService->start();

  btAdvertise();
  Serial.println("Bluetooth initted");
}

void btAdvertise() {
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();

  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);
  pAdvertising->setMinPreferred(0x12);

  BLEDevice::startAdvertising();
  Serial.println("Characteristic defined! Now you can read it in your phone!");

}