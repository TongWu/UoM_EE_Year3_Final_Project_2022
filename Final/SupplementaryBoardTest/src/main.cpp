#include <Arduino.h>
#include "ArduinoBLE.h"
#include "VL53L1X.h"
#include "LSM6DS33.h"

// float sensorData[3] = {0, 0, 0};
float distance = 0;
float accValue = 0;
float gyroValue = 0;
/* 0: ToF
 * 1. Acc
 * 2. Gyro
 */
bool sensorFlag[3] = {false, false, false};
bool eventFlag[7] = {false, false, false, false, false, false, false};
/*
 * 0: tof & acc & gyro
 * 1: tof & acc
 * 2: tof & gyro
 * 3: acc & gyro
 * 4: tof
 * 5: acc
 * 6: gyro
 */
int ToF_Count = 0;
int accCount = 0;
int gyroCount = 0;

void setup() {
	Serial.begin(9600);
	while (!Serial)
		delay(1);
	BLE.begin();
	BLE.setLocalName("Frame");
	vl53_Setup();
	LSM6DS33_Setup();
}

void getSensorData() {
	LSM6DS33();
	setPrevious();
	for (int i = 0; i < 15; ++i) {
		LSM6DS33();
		distance += getDistance() / 10.0;
		accValue += AccelCalculateDifference();
		gyroValue += getGyro();
	}
	gyroValue = gyroValue / 15;
	distance = distance / 15;
	accValue = accValue / 15;
	Serial.print("ToF: ");
	Serial.println(distance);
	Serial.print("Acc: ");
	Serial.println(accValue);
	Serial.print("Gyro: ");
	Serial.println(gyroValue);
}

void setFlag() {
	if (distance <= 5)
		sensorFlag[0] = true;
	if (accValue >= 2)
		sensorFlag[1] = true;
	if (gyroValue >= 2)
		sensorFlag[2] = true;
}

void decisionTree() {
	if (sensorFlag[0] && sensorFlag[1] && sensorFlag[2]) {
		eventFlag[0] = true;
		Serial.println("tof & acc & gyro");
	}
	else if(sensorFlag[0]) {
		if (sensorFlag[1]) {
			eventFlag[1] = true;
			Serial.println("tof & acc");
		} else if (sensorFlag[2]) {
			eventFlag[2] = true;
			Serial.println("tof & gyro");
		} else {
			ToF_Count++;
			if (ToF_Count >= 30) {
				eventFlag[4] = true;
				Serial.println("tof");
				ToF_Count = 0;
			}
		}
	}
	else if(sensorFlag[1]) {
		if (sensorFlag[0]) {
			eventFlag[1] = true;
			Serial.println("tof & acc");
		} else if (sensorFlag[2]) {
			eventFlag[3] = true;
			Serial.println("acc & gyro");
		} else {
			accCount++;
			if (accCount >= 30) {
				eventFlag[5] = true;
				Serial.println("acc");
				accCount = 0;
			}
		}
	}
	else if(sensorFlag[2]) {
		if (sensorFlag[0]) {
			eventFlag[2] = true;
			Serial.println("tof & gyro");
		} else if (sensorFlag[1]) {
			eventFlag[3] = true;
			Serial.println("acc & gyro");
		} else {
			gyroCount++;
			if (gyroCount >= 30) {
				eventFlag[6] = true;
				Serial.println("gyro");
				gyroCount = 0;
			}
		}
	}
}

void BLEConnect() {
	if (eventFlag[0] || eventFlag[1] || eventFlag[2] || eventFlag[3] || eventFlag[4] || eventFlag[5] || eventFlag[6]) {
		Serial.println("Start connect");
		BLE.scanForUuid("19B1");
		/// delay(5000);
		BLEDevice uno = BLE.available();
		if (uno) {
			Serial.print("Found ");
			Serial.print(uno.address());
			Serial.print(" '");
			Serial.print(uno.localName());
			Serial.print("' ");
			Serial.print(uno.advertisedServiceUuid());
			Serial.println();
			if (uno.localName() != "uno") {
				return;
			}
			BLE.stopScan();
			Serial.println("Connecting ...");
			if (uno.connect()) {
				Serial.println("Connected");
			} else {
				Serial.println("Failed to connect!");
				return;
			}
			// discover peripheral attributes
			Serial.println("Discovering attributes ...");
			// retrieve the LED characteristic
			BLECharacteristic tof_acc_gyro = uno.characteristic("19b10001-e8f2-537e-4f6c-d104768a1214");
			BLECharacteristic tof_acc = uno.characteristic("19B10002-E8F2-537E-4F6C-D104768A1214");
			BLECharacteristic tof_gyro = uno.characteristic("19B10003-E8F2-537E-4F6C-D104768A1214");
			BLECharacteristic acc_gyro = uno.characteristic("19b10004-e8f2-537e-4f6c-d104768a1214");
			BLECharacteristic tof = uno.characteristic("19B10005-E8F2-537E-4F6C-D104768A1214");
			BLECharacteristic acc = uno.characteristic("19B10006-E8F2-537E-4F6C-D104768A1214");
			BLECharacteristic gyro = uno.characteristic("19B10007-E8F2-537E-4F6C-D104768A1214");
			while (uno.connected()) {
				if (eventFlag[0])
					tof_acc_gyro.writeValue((byte)0x01);
				else if (eventFlag[1])
					tof_acc.writeValue((byte)0x01);
				else if (eventFlag[2])
					tof_gyro.writeValue((byte)0x01);
				else if (eventFlag[3])
					acc_gyro.writeValue((byte)0x01);
				else if (eventFlag[4])
					tof.writeValue((byte)0x01);
				else if (eventFlag[5])
					acc.writeValue((byte)0x01);
				else if (eventFlag[6])
					gyro.writeValue((byte)0x01);
				Serial.println("Value sent");
				// uno.disconnect();
				// Serial.println("Disconnected");
				/*for (bool & i : eventFlag) {
					i = false;
				}*/
			}
			delay(500);
		}
	}
	delay(500);
}

void clean() {
	for (bool & i : sensorFlag) {
		i = false;
	}
}

void loop() {
	getSensorData();
	setFlag();
	decisionTree();
	BLEConnect();
	clean();
	// Total delay 3 seconds
}