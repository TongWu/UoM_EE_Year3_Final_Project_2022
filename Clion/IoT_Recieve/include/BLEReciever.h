//
// Created by TedWu on 2022/3/19.
//

#ifndef IOT_RECIEVE_BLERECIEVER_H
#define IOT_RECIEVE_BLERECIEVER_H
#include <ArduinoBLE.h>

BLEService proximityService("19B10000-E8F2-537E-4F6C-D104768A1214"); // Bluetooth® Low Energy LED Service
// Bluetooth® Low Energy LED Switch Characteristic - custom 128-bit UUID, read and writable by central
BLEByteCharacteristic proximityCharacteristic("19B10001-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite);

void BLE_Setup() {
	// begin initialization
	if (!BLE.begin()) {
		Serial.println("starting Bluetooth® Low Energy module failed!");

		while (1);
	}

	// set advertised local name and service UUID:
	BLE.setLocalName("Central");
	BLE.setAdvertisedService(proximityService);

	// add the characteristic to the service
	proximityService.addCharacteristic(proximityCharacteristic);

	// add service
	BLE.addService(proximityService);

	// set the initial value for the characeristic:
	proximityCharacteristic.writeValue(0);

	// start advertising
	BLE.advertise();

	Serial.println("BLE Central");
}

void BLE_Control() {
	// listen for Bluetooth® Low Energy peripherals to connect:
	BLEDevice peripheral = BLE.central();

	// if a central is connected to peripheral:
	if (peripheral) {
		Serial.print("Connected to central: ");
		// print the central's MAC address:
		Serial.println(peripheral.address());

		// while the central is still connected to peripheral:
		while (peripheral.connected()) {
			// if the remote device wrote to the characteristic,
			// use the value to control the LED:
			if (proximityCharacteristic.written()) {
				if (proximityCharacteristic.value()) {   // any value other than 0
					Serial.println("Proximity Unsafe");
				} else {                              // a 0 value
					Serial.println(F("Safe"));
				}
			}
		}

		// when the central disconnects, print it out:
		Serial.print(F("Disconnected from peripheral: "));
		Serial.println(peripheral.address());
	}
}

#endif //IOT_RECIEVE_BLERECIEVER_H
