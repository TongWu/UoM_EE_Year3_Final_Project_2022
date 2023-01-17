#include <ArduinoBLE.h>

BLEService frame("19B10000-E8F2-537E-4F6C-D104768A1214"); // Bluetooth® Low Energy Service

// Bluetooth® Low Energy Characteristic - custom 128-bit UUID, read and writable by central
BLEByteCharacteristic proximityCharacteristic("19B10001-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite);
BLEByteCharacteristic accCharacteristic("19B10002-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite);
BLEByteCharacteristic gyroCharacteristic("19B10003-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite);

void setup() {
	Serial.begin(9600);
	while (!Serial);

	// begin initialization
	if (!BLE.begin()) {
		Serial.println("starting Bluetooth® Low Energy module failed!");

		while (1);
	}

	// set advertised local name and service UUID:
	BLE.setLocalName("Nano");
	BLE.setAdvertisedService(frame);

	// add the characteristic to the service
	frame.addCharacteristic(proximityCharacteristic);
	frame.addCharacteristic(accCharacteristic);
	frame.addCharacteristic(gyroCharacteristic);

	// add service
	BLE.addService(frame);

	// set the initial value for the characeristic:
	proximityCharacteristic.writeValue(0);

	// start advertising
	BLE.advertise();

	Serial.println("BLE Central");
}

void loop() {
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
				if (proximityCharacteristic.value())  // any value other than 0
					Serial.println("Proximity");
			}
			if (accCharacteristic.written()) {
				if (accCharacteristic.value())
					Serial.println("Accelerometer");
			}
			if (gyroCharacteristic.written()) {
				if (gyroCharacteristic.value())
					Serial.println("Gyro");
			}
		}

		// when the central disconnects, print it out:
		Serial.print(F("Disconnected from peripheral: "));
		Serial.println(peripheral.address());
	}
}
