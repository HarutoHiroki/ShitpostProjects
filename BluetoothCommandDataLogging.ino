#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>

// ==================================== Bluetooth Stuff ====================================
BLEServer* pServer = NULL;
BLECharacteristic* pTxCharacteristic = NULL;
BLECharacteristic* pRxCharacteristic = NULL;
bool deviceConnected = false;
bool oldDeviceConnected = false;

// ===================================== Lovense Stuff =====================================
// Device stuff
#define DEVICE_NAME             "LVS-ESP32-Gush"                        // All Lovense toys start with "LVS-" (This doesn't really matter)
#define DEVICE_TYPE             "ED:45:0082059AD3BD;"                    // DeviceType:Firmware ver:BLE Address (Somehow this is what the new Lovense app use to identify the toy)
#define SERVICE_UUID            "45440001-0023-4BD4-BBD5-A6920E4C5653"  // Service UUID
#define CHARACTERISTIC_RX_UUID  "45440002-0023-4BD4-BBD5-A6920E4C5653"  // Write Characteristic UUID
#define CHARACTERISTIC_TX_UUID  "45440003-0023-4BD4-BBD5-A6920E4C5653"  // Notify Characteristic UUID

// General Device Status Stuff
int light = 1;
char ledStatus[8]; // Light:1;
int autoStop = 1; // Conserve battery on disconnect
int reconnectLastLv = 0; // Reset by default
char autoSwithState[15]; // AutoSwith:1:0;

// Vibrator Motors
int vibration[3] = {0, 0, 0};
int rotation = 0;
int rotationDirection = 0; // 0 = Clockwise (Default), 1 = Counter Clockwise

// Restoring last vibration and rotation settings
int lastVibration[3] = {0, 0, 0};
int lastRotation = 0;

// ====================================== Code Stuff ======================================
void UpdateVibe(void) {
  Serial.println("=================================");
  Serial.println("          VIBRATOR DATA          ");
  Serial.println("=================================");
  for (int i = 0; i < sizeof(vibration) / sizeof(vibration[0]); i++) {
    Serial.print("Vibrator ");
    Serial.print(i + 1);
    Serial.print(" level: ");
    Serial.println(vibration[i]);
  }
  Serial.print("Rotation level: ");
  Serial.println(rotation);
  Serial.println("---------------------------------");

  // Save last vibration and rotation settings
  for (int i = 0; i < sizeof(vibration) / sizeof(vibration[0]); i++) {
    lastVibration[i] = vibration[i];
  }
  lastRotation = rotation;
}

void UpdateLight(const String lightStatus) {
  if (lightStatus == "on") {
    light = 1;
  } else if (lightStatus == "off") {
    light = 0;
  } else {
    Serial.println("Invalid light status received");
    return;
  }
  sprintf(ledStatus, "Light:%d;", light);
  Serial.print("Light updated to: ");
  Serial.println(light == 1 ? "on" : "off");
  Serial.println();
}

void UpdateAutoSwith(const String astop, const String recon) {
  if (astop == "On") {
    autoStop = 1;
  } else if (astop == "Off") {
    autoStop = 0;
  } else {
    Serial.println("Invalid Auto Stop on Disconnect value");
    return;
  }

  if (recon == "On") {
    reconnectLastLv = 1;
  } else if (recon == "Off") {
    reconnectLastLv = 0;
  } else {
    Serial.println("Invalid Reconnect Restore value");
    return;
  }
  sprintf(autoSwithState, "AutoSwith:%d:%d;", autoStop, reconnectLastLv);
  Serial.print("AutoSwith updated to: ");
  Serial.println(autoSwithState);
  Serial.println();
}

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
      BLEDevice::startAdvertising();
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
    }
};

class MySerialCallbacks: public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    static uint8_t messageBuf[64];
    assert(pCharacteristic == pRxCharacteristic);
    String value = pRxCharacteristic->getValue();
    
    if (value.length() > 0) {
      Serial.print("> Command Recieved: ");
      for (int i = 0; i < value.length(); i++)
        Serial.print(value[i]);

      Serial.println();
      Serial.println();
    }

    
    if (value == "DeviceType;") { // Device type (Edge)
      Serial.println("> Responding to Device Enquiry");
      memmove(messageBuf, DEVICE_TYPE, 18);
      pTxCharacteristic->setValue(messageBuf, 18);
      pTxCharacteristic->notify();
      
    } else if (value == "Battery;") { // Battery always at 69 cuz funny
      memmove(messageBuf, "69;", 3);
      pTxCharacteristic->setValue(messageBuf, 3);
      pTxCharacteristic->notify();

    } else if (value == "PowerOff;") { // Fake power off
      memmove(messageBuf, "OK;", 3);
      pTxCharacteristic->setValue(messageBuf, 3);
      pTxCharacteristic->notify();

    } else if (value.indexOf("Status:") != -1) { // Device status OK
      memmove(messageBuf, "2;", 2);
      pTxCharacteristic->setValue(messageBuf, 3);
      pTxCharacteristic->notify();

    } else if (value.indexOf("GetAS") != -1) { // Get Auto Swith
      memmove(messageBuf, autoSwithState, 14);
      pTxCharacteristic->setValue(messageBuf, 3);
      pTxCharacteristic->notify();

    } else if (value.indexOf("AutoSwith:") != -1) { // Set Auto Swith "AutoSwith:On:Off;"
      String autoSwitchCommand = value.substring(10, value.length() - 1);
      String autoStop = autoSwitchCommand.substring(0, autoSwitchCommand.indexOf(":"));
      String reconnect = autoSwitchCommand.substring(autoSwitchCommand.indexOf(":") + 1);
      UpdateAutoSwith(autoStop, reconnect);
      memmove(messageBuf, "OK;", 3);
      pTxCharacteristic->setValue(messageBuf, 3);
      pTxCharacteristic->notify();

    } else if (value.indexOf("GetLight") != -1) { // Get device light
      memmove(messageBuf, ledStatus, 8);
      pTxCharacteristic->setValue(messageBuf, 3);
      pTxCharacteristic->notify();

    } else if (value.indexOf("Light:") != -1) { // Set device light
      UpdateLight(value.substring(6, value.length() - 1));
      memmove(messageBuf, "OK;", 3);
      pTxCharacteristic->setValue(messageBuf, 3);
      pTxCharacteristic->notify();

    } else if (value.indexOf("Vibrate:") != -1) { // Vibrate motor
      vibration[0] = std::atoi(value.substring(8).c_str());
      memmove(messageBuf, "OK;", 3);
      pTxCharacteristic->setValue(messageBuf, 3);
      pTxCharacteristic->notify();
      UpdateVibe();

    } else if (value.indexOf("Vibrate1:") != -1) { // Vibrate motor 1
      vibration[0] = std::atoi(value.substring(9).c_str());
      memmove(messageBuf, "OK;", 3);
      pTxCharacteristic->setValue(messageBuf, 3);
      pTxCharacteristic->notify();
      UpdateVibe();

    } else if (value.indexOf("Vibrate2:") != -1) { // Vibrate motor 2
      vibration[1] = std::atoi(value.substring(9).c_str());
      memmove(messageBuf, "OK;", 3);
      pTxCharacteristic->setValue(messageBuf, 3);
      pTxCharacteristic->notify();
      UpdateVibe();
      
    } else if (value == "RotateChange;") { // Rotation toys
      memmove(messageBuf, "OK;", 3);
      rotationDirection = !rotationDirection;
      pTxCharacteristic->setValue(messageBuf, 3);
      pTxCharacteristic->notify();
      UpdateVibe();

    } else if (value.indexOf("Rotate:") != -1) { // Rotate motor
      rotation = std::atoi(value.substring(7).c_str());
      memmove(messageBuf, "OK;", 3);
      pTxCharacteristic->setValue(messageBuf, 3);
      pTxCharacteristic->notify();
      UpdateVibe();

    } else if (value.indexOf("Mply:") != -1) { // Lapis, Flexer, Ridge support // Mply:number:number:number;
      // first value always vibration
      int v1 = std::atoi(value.substring(5, value.indexOf(":", 5)).c_str());
      vibration[0] = v1 >= 0 ? v1 : vibration[0];
      // check device type 
      String deviceType = DEVICE_TYPE;
      if (deviceType.indexOf("U") != -1) { // Lapis
        int v2 = std::atoi(value.substring(value.indexOf(":", 5) + 1, value.indexOf(":", value.indexOf(":", 5) + 1)).c_str());
        int v3 = std::atoi(value.substring(value.indexOf(":", value.indexOf(":", 5) + 1) + 1, value.length() - 1).c_str());
        vibration[1] = v2 >= 0 ? v2 : vibration[1];
        vibration[2] = v3 >= 0 ? v3 : vibration[2];
      } else if (deviceType.indexOf("EI") != -1) { // Flexer
        int v2 = std::atoi(value.substring(value.indexOf(":", 5) + 1, value.length() - 1).c_str());
        int rot = std::atoi(value.substring(value.indexOf(":", value.indexOf(":", 5) + 1) + 1, value.length() - 1).c_str());
        vibration[1] = v2 >= 0 ? v2 : vibration[1];
        rotation = rot >= 0 ? rot : rotation;
      } else if (deviceType.indexOf("EL") != -1) { // Ridge
        int rot = std::atoi(value.substring(value.indexOf(":", 5) + 1, value.length() - 1).c_str());
        rotation = rot >= 0 ? rot : rotation;
      }
      memmove(messageBuf, "OK;", 3);
      pTxCharacteristic->setValue(messageBuf, 3);
      pTxCharacteristic->notify();
      UpdateVibe();
      
    } else if (value.indexOf("Multiply:") != -1) { // Gemini Support // Multiply:number:number;
      int v1 = std::atoi(value.substring(9, value.indexOf(":", 9)).c_str());
      int v2 = std::atoi(value.substring(value.indexOf(":", 9) + 1, value.length() - 1).c_str());
      vibration[0] = v1 >= 0 ? v1 : vibration[0];
      vibration[1] = v2 >= 0 ? v2 : vibration[1];
      memmove(messageBuf, "OK;", 3);
      pTxCharacteristic->setValue(messageBuf, 3);
      pTxCharacteristic->notify();
      UpdateVibe();

    } else {
      Serial.println("!!!!! Unknown request !!!!!");
      Serial.println();
      memmove(messageBuf, "ERR;", 4);
      pTxCharacteristic->setValue(messageBuf, 4);
      pTxCharacteristic->notify();
    }
  }
};

void setup() {
  // ===================================== Lovense Stuff =====================================
  Serial.begin(115200);

  // Init some values
  sprintf(ledStatus, "Light:%d;", light);
  sprintf(autoSwithState, "AutoSwith:%d:%d;", autoStop, reconnectLastLv);

  // Create the BLE Device
  BLEDevice::init(DEVICE_NAME);

  // Create the BLE Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create a BLE Characteristics
  pTxCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_TX_UUID,
                      BLECharacteristic::PROPERTY_NOTIFY
                    );
  pTxCharacteristic->addDescriptor(new BLE2902());

  pRxCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_RX_UUID,
                      BLECharacteristic::PROPERTY_WRITE  |
                      BLECharacteristic::PROPERTY_WRITE_NR
                    );
  pRxCharacteristic->setCallbacks(new MySerialCallbacks());

  // Start the service
  pService->start();

  // Start advertising
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(false);
  pAdvertising->setMinPreferred(0x0);  // set value to 0x00 to not advertise this parameter
  BLEDevice::startAdvertising();
  Serial.println("Waiting a client connection to notify...");
}

void loop() {
    // disconnecting
    if (!deviceConnected && oldDeviceConnected) {
        delay(100); // give the bluetooth stack the chance to get things ready
        pServer->startAdvertising(); // restart advertising
        Serial.println("start advertising");
        oldDeviceConnected = deviceConnected;

        // Check if the setting for stop on disconnect is enabled
        if (autoStop) {
          // Stop all motors
          for (int i = 0; i < sizeof(vibration) / sizeof(vibration[0]); i++) {
            vibration[i] = 0;
          }
          rotation = 0;
          UpdateVibe();
        }
    }
    // connecting
    if (deviceConnected && !oldDeviceConnected) {
        // do stuff here on connecting
        oldDeviceConnected = deviceConnected;

        // Check if the setting for reconnect last level is enabled
        if (reconnectLastLv) {
          // Restore last vibration and rotation settings
          for (int i = 0; i < sizeof(vibration) / sizeof(vibration[0]); i++) {
            vibration[i] = lastVibration[i];
          }
          rotation = lastRotation;
          UpdateVibe();
        }
    }
}