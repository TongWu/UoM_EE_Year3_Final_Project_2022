/*
 * This program is to connect the main board (Arduino Uno Wi-Fi Rev.2) with the supplementary board (Arduino Nano 33 IOT)
 * This program is for the supplementary board as a sender
 */

#include "Arduino.h"
#include "ArduinoBLE.h"
/*#include "Adafruit_LIS3MDL.h"       // Mag
#include "Adafruit_LSM6DS33.h"      // Acc & Gyro*/
#include "VL53L1X.h"       // ToF

const char* deviceServiceUuid = "19b10000-e8f2-537e-4f6c-d104768a1214";
const char* deviceServiceCharacteristicUuid = "19b10001-e8f2-537e-4f6c-d104768a1214";

BLEService proximityService(deviceServiceUuid);
BLEByteCharacteristic isProximitySafe(deviceServiceCharacteristicUuid, BLERead | BLEWrite);

/*BLEService imuService(deviceServiceUuid);
BLEByteCharacteristic imuCharacteristic(deviceServiceCharacteristicUuid, BLERead | BLEWrite);*/

long previousMillis = 0;

void decisionTree() {
	float distance = getDistance() / 10.0;
	bool isSafe = false;
	if (distance <= 5 && distance >= 0) {
		isSafe = false;
	}
	else isSafe = true;
	isProximitySafe.writeValue(isSafe);
}

void setup() {
	Serial.begin(9600);

	vl53_Setup();

	if (!BLE.begin()) {
		Serial.println("- Starting Bluetooth® Low Energy module failed!");
		while (1);
	}
	BLE.setLocalName("Nano 33 IoT");

	// Set up the proximity monitor service
	BLE.setAdvertisedService(proximityService);
	proximityService.addCharacteristic(isProximitySafe);
	BLE.addService(proximityService);
	isProximitySafe.writeValue(true);

	// Start advertising
	BLE.advertise();
}

void loop() {
	// wait for central device
	BLEDevice central = BLE.central();
	if (central) {
		Serial.print("Connected to central: ");
		Serial.println(central.address());

		while (central.connected()) {
			long currentMillis = millis();
			if (currentMillis - previousMillis >= 500) {
				previousMillis = currentMillis;
				decisionTree();
			}
		}
		Serial.println("Disconnected");
	}
}