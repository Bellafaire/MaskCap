/*
   Based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleScan.cpp
   Ported to Arduino ESP32 by Evandro Copercini
*/

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEServer.h>
#include <BLEAdvertisedDevice.h>
#include <Stepper.h>

#define SERVICE_UUID        "ca575296-1972-43c1-8475-e4bb29c7b3f5"


boolean retracted = false;
int scanTime = 1; //In seconds
BLEScan* pBLEScan;
boolean detected = false;


// Number of steps per output rotation
const int stepsPerRevolution = 600;

// Create Instance of Stepper library
//Stepper myStepper(stepsPerRevolution, 5, 4, 3, 2);
//Stepper rightStepper(stepsPerRevolution, 32, 33, 25, 26);
Stepper rightStepper(stepsPerRevolution, 32, 33, 25, 26);
//Stepper leftStepper(stepsPerRevolution, 19, 18, 17, 16);
Stepper leftStepper(stepsPerRevolution, 22, 23, 21, 19);


class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
    /**
        Called for each advertising BLE server.
    */
    void onResult(BLEAdvertisedDevice advertisedDevice) {
      //      Serial.print("BLE Advertised Device found: ");
      //      Serial.println(advertisedDevice.toString().c_str());

      // We have found a device, let us now see if it contains the service we are looking for.
      if (advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(BLEUUID(SERVICE_UUID))) {
        BLEDevice::getScan()->stop();
        detected = true;
      }
    }
};


void setup() {
  Serial.begin(115200);
  Serial.println("Scanning...");
  pinMode(4, OUTPUT);
  digitalWrite(4, LOW);
  rightStepper.setSpeed(25);
  leftStepper.setSpeed(25);
  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan(); //create new scan
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(false); //active scan uses more power, but get results faster
  pBLEScan->setInterval(100);
  pBLEScan->setWindow(99);  // less or equal setInterval value


  BLEDevice::init("CapMask");
  BLEServer *pServer = BLEDevice::createServer();
  BLEService *pService = pServer->createService(SERVICE_UUID);

  pService->start();
  // BLEAdvertising *pAdvertising = pServer->getAdvertising();  // this still is working for backward compatibility
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
  Serial.println("Characteristic defined! Now you can read it in your phone!");
}

void raiseLeft(void * pvParameters) {
  leftStepper.setSpeed(20);
  leftStepper.step(-1500);
  vTaskDelete(NULL);
}

void lowerLeft(void * pvParameters) {
  leftStepper.setSpeed(35);
  leftStepper.step(1000);
  vTaskDelete(NULL);
}
void raiseRight(void * pvParameters) {
  rightStepper.setSpeed(20);
  rightStepper.step(-1000);
  vTaskDelete(NULL);
}

void lowerRight(void * pvParameters) {
  rightStepper.setSpeed(35);
  rightStepper.step(1500);
  vTaskDelete(NULL);
}

void raiseMask() {

  TaskHandle_t xRaiseLeft = NULL;
  TaskHandle_t xRaiseRight = NULL;
  xTaskCreatePinnedToCore( raiseRight, "raise_right", 4096, (void *) 1 , tskIDLE_PRIORITY, &xRaiseRight, 0 );
  xTaskCreatePinnedToCore( raiseLeft, "raise_left", 4096, (void *) 1 , tskIDLE_PRIORITY, &xRaiseLeft, 0 );

  delay(15000);
}

void lowerMask() {
  TaskHandle_t xLowerLeft = NULL;
  TaskHandle_t xLowerRight = NULL;
  xTaskCreatePinnedToCore( lowerRight, "raise_right", 4096, (void *) 1 , tskIDLE_PRIORITY, &xLowerRight, 0 );
  xTaskCreatePinnedToCore( lowerLeft, "raise_left", 4096, (void *) 1 , tskIDLE_PRIORITY, &xLowerLeft, 0 );

  delay(15000);
}

void loop() {
  // put your main code here, to run repeatedly:
  detected = false;
  BLEScanResults foundDevices = pBLEScan->start(scanTime, false);
  Serial.print("Devices found: ");
  Serial.println(foundDevices.getCount());
  Serial.println("Scan done!");
  pBLEScan->clearResults();   // delete results fromBLEScan buffer to release memory
  delay(1000);
    if (retracted && detected) {
      lowerMask();
      retracted = false;
    } else if (!retracted && !detected) {
      raiseMask(); 
      retracted = true;
    }

}
