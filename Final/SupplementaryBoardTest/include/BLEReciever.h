//
// Created by TedWu on 2022/3/19.
//

#ifndef MAINBOARDBLE_BLERECIEVER_H
#define MAINBOARDBLE_BLERECIEVER_H

#include "Arduino.h"
#include <ArduinoBLE.h>

BLEService SupplementaryFrame("19B10000-E8F2-537E-4F6C-D104768A1214"); // Bluetooth® Low Energy LED Service
BLEService SupplementaryBack("19B20000-E8F2-537E-4F6C-D104768A1214");
BLEService SupplementaryFront("19B30000-E8F2-537E-4F6C-D104768A1214");

// Bluetooth® Low Energy LED Switch Characteristic - custom 128-bit UUID, read and writable by central
BLEByteCharacteristic proximityCharacteristic("19B10001-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite);
BLEByteCharacteristic accCharacteristic("19B10002-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite);
BLEByteCharacteristic gyroCharacteristic("19B10003-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite);
// BLEDevice peripheral;

bool Supplementary1Event[3] = {
		false, // 1: proximity
		false, // 1: acc
		false, // 1: gyro
};

void BLERecieverSetup() {
	// begin initialization
	if (!BLE.begin()) {
		Serial.println("starting Bluetooth® Low Energy module failed!");

		while (1);
	}

	// set advertised local name and service UUID:
	BLE.setLocalName("Central");
	BLE.setAdvertisedService(SupplementaryFrame);

	// add the characteristic to the service
	SupplementaryFrame.addCharacteristic(proximityCharacteristic);
	SupplementaryFrame.addCharacteristic(accCharacteristic);
	SupplementaryFrame.addCharacteristic(gyroCharacteristic);

	// add service
	BLE.addService(SupplementaryFrame);

	// set the initial value for the characeristic:
	proximityCharacteristic.writeValue(0);
	accCharacteristic.writeValue(0);
	gyroCharacteristic.writeValue(0);

	// start advertising
	BLE.advertise();

	Serial.println("BLE Central");
}

void BLEReciever() {
	// listen for Bluetooth® Low Energy peripherals to connect:
	BLEDevice peripheral = BLE.central();

	// if a central is connected to peripheral:
	if (peripheral) {
		Serial.print("Connected to central: ");
		// print the central's MAC address:
		Serial.println(peripheral.address());

		// while the central is still connected to peripheral:
		if (peripheral.connected()) {
			// if the remote device wrote to the characteristic,
			// use the value to control the LED:
			if (proximityCharacteristic.written()) {
				if (proximityCharacteristic.value()) {  // any value other than 0
					Serial.println("Proximity");
					Supplementary1Event[1] = true;
				}
			}
			if (accCharacteristic.written()) {
				if (accCharacteristic.value()) {
					Serial.println("Accelerometer");
					Supplementary1Event[2] = true;
				}
			}
			if (gyroCharacteristic.written()) {
				if (gyroCharacteristic.value()) {
					Serial.println("Gyro");
					Supplementary1Event[3] = true;
				}
			}
		}

		// when the central disconnects, print it out:
		// Serial.print(F("Disconnected from peripheral: "));
		// Serial.println(peripheral.address());
	}
}

#endif //MAINBOARDBLE_BLERECIEVER_H
