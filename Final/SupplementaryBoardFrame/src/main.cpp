#include "Arduino.h"
#include <ArduinoBLE.h>
#include "VL53L1X.h"
#include "LSM6DS33.h"

// variables for button

void setup() {
	Serial.begin(9600);
	while (!Serial);
	vl53_Setup();
	LSM6DS33_Setup();

	// initialize the Bluetooth® Low Energy hardware
	BLE.begin();
	BLE.setLocalName("Frame");
	Serial.println("Bluetooth® Low Energy");

	// start scanning
	BLE.scanForUuid("19B1");

}

void loop() {
	// check if a peripheral has been discovered
	BLEDevice central = BLE.available();

	if (central) {
		// discovered a peripheral, print out address, local name, and advertised service
		Serial.print("Found ");
		Serial.print(central.address());
		Serial.print(" '");
		Serial.print(central.localName());
		Serial.print("' ");
		Serial.print(central.advertisedServiceUuid());
		Serial.println();

		if (central.localName() != "Central") {
			return;
		}

		// stop scanning
		BLE.stopScan();

		controlLed(central);

		// peripheral disconnected, start scanning again
		BLE.scanForUuid("19B1");
	}
}

void controlLed(BLEDevice central) {
	// connect to the peripheral
	Serial.println("Connecting ...");

	if (central.connect()) {
		Serial.println("Connected");
	} else {
		Serial.println("Failed to connect!");
		return;
	}

	// discover peripheral attributes
	Serial.println("Discovering attributes ...");
	if (central.discoverAttributes()) {
		Serial.println("Attributes discovered");
	} else {
		Serial.println("Attribute discovery failed!");
		central.disconnect();
		return;
	}

	// retrieve the LED characteristic
	BLECharacteristic proximity = central.characteristic("19b10001-e8f2-537e-4f6c-d104768a1214");
	BLECharacteristic acc = central.characteristic("19B10002-E8F2-537E-4F6C-D104768A1214");
	BLECharacteristic gyro = central.characteristic("19B10003-E8F2-537E-4F6C-D104768A1214");

	if (!proximity) {
		Serial.println("Peripheral does not have Proximity characteristic!");
		central.disconnect();
		return;
	} else if (!proximity.canWrite()) {
		Serial.println("Peripheral does not have a writable Proximity characteristic!");
		central.disconnect();
		return;
	}
	if (!acc) {
		Serial.println("Peripheral does not have accgyro characteristic!");
		central.disconnect();
		return;
	} else if (!acc.canWrite()) {
		Serial.println("Peripheral does not have a accgyro  characteristic!");
		central.disconnect();
		return;
	}
	if (!gyro) {
		Serial.println("Peripheral does not have accgyro characteristic!");
		central.disconnect();
		return;
	} else if (!gyro.canWrite()) {
		Serial.println("Peripheral does not have a accgyro  characteristic!");
		central.disconnect();
		return;
	}

	while (central.connected()) {
		LSM6DS33();
		// while the peripheral is connected
		float distance = getDistance() / 10.0;
		float gyroValue = getGyro();
		float accValue = AccelCalculateDifference();
		if (distance >= 5) {
			proximity.writeValue((byte)0x00);
		}
		else {
			proximity.writeValue((byte)0x01);
		}
		Serial.println(gyroValue);
		if (gyroValue >= 3) {
			gyro.writeValue((byte)0x01);
		} else {
			gyro.writeValue((byte)0x00);
		}
		if (accValue >= 3) {
			acc.writeValue((byte) 0x01);
		} else {
			acc.writeValue((byte)0x00);
		}
	}
	if (!central.disconnect())
		Serial.println("Central disconnected");
	delay(600);
}