#include <Arduino.h>
#include "buzzer.h"
#include "SIM7600CE.h"
#include "Fingerprint.h"
#include <ArduinoBLE.h>

/*Set up BLE Service and characteristics*/
BLEService Supplementary("19B1");
BLEByteCharacteristic tof_acc_gyro("19B10001-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite);
BLEByteCharacteristic tof_acc("19B10002-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite);
BLEByteCharacteristic tof_gyro("19B10003-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite);
BLEByteCharacteristic acc_gyro("19B10004-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite);
BLEByteCharacteristic tof("19B10005-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite);
BLEByteCharacteristic acc("19B10006-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite);
BLEByteCharacteristic gyro("19B10007-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite);
BLEDevice Frame;
BLEDevice Front;
BLEDevice Back;

int SIM_TextTime = 0;

void BLERecieverSetup() {
	// begin initialization
	if (!BLE.begin()) {
		Serial.println("starting Bluetooth® Low Energy module failed!");

		while (1);
	}
	// set advertised local name and service UUID:
	BLE.setLocalName("uno");
	BLE.setAdvertisedService(Supplementary);
	// add the characteristic to the service
	Supplementary.addCharacteristic(tof_acc_gyro);
	Supplementary.addCharacteristic(tof_acc);
	Supplementary.addCharacteristic(tof_gyro);
	Supplementary.addCharacteristic(acc_gyro);
	Supplementary.addCharacteristic(tof);
	Supplementary.addCharacteristic(acc);
	Supplementary.addCharacteristic(gyro);
	// add service
	BLE.addService(Supplementary);
	// set the initial value for the characeristic:
	tof_acc_gyro.writeValue(0);
	tof_acc.writeValue(0);
	tof_gyro.writeValue(0);
	acc_gyro.writeValue(0);
	tof.writeValue(0);
	acc.writeValue(0);
	gyro.writeValue(0);
	// start advertising
	BLE.advertise();
	Serial.println("BLE Central");
}

void BLEReciever() {
	BLEDevice IoT = BLE.central();
	int board = 0;
	if (IoT) {
		Serial.print("Connected to IoT: ");
		// print the central's MAC address:
		Serial.println(IoT.localName());
		Serial.println(IoT.address());
		if (IoT.address() == "7c:9e:bd:68:6f:12")
			board = 1;
		else if (IoT.localName() == "Front")
			board = 2;
		else if (IoT.localName() == "Back")
			board = 3;
		else
			board = 0;
		if (board == 0) {
			return;
		}
		// while the central is still connected to peripheral:
		while (!IoT.connected()) {
			// if the remote device wrote to the characteristic,
			// use the value to control the LED:
			if (tof_acc_gyro.written()) {
				if (tof_acc_gyro.value()) {  // any value other than 0
					Serial.print(IoT.localName());
					Serial.println(" event: ");
					Serial.println("tof & acc & gyro");
					if (SIM_TextTime <= 0) {
						SIM_TextTime = 20;
						SendMessage(board, 'ToF and accelerometer and gyroscope');
					}
					SIM_TextTime--;
				}
			}
			if (tof_acc.written()) {
				if (tof_acc.value()) {  // any value other than 0
					Serial.print(IoT.localName());
					Serial.println(" event: ");
					Serial.println("tof & acc");
					if (SIM_TextTime <= 0) {
						SIM_TextTime = 20;
						SendMessage(board, 'ToF and accelerometer');
					}
					SIM_TextTime--;
				}
			}
			if (tof_gyro.written()) {
				if (tof_gyro.value()) {  // any value other than 0
					Serial.print(IoT.localName());
					Serial.println(" event: ");
					Serial.println("tof & gyro");
					if (SIM_TextTime <= 0) {
						SIM_TextTime = 20;
						SendMessage(board, 'ToF and gyroscope');
					}
					SIM_TextTime--;
				}
			}
			if (acc_gyro.written()) {
				if (acc_gyro.value()) {  // any value other than 0
					Serial.print(IoT.localName());
					Serial.println(" event: ");
					Serial.println("acc & gyro");
					if (SIM_TextTime <= 0) {
						SIM_TextTime = 20;
						SendMessage(board, 'accelerometer and gyroscope');
					}
					SIM_TextTime--;
				}
			}
			if (tof.written()) {
				if (tof.value()) {  // any value other than 0
					Serial.print(IoT.localName());
					Serial.println(" event: ");
					Serial.println("tof");
					if (SIM_TextTime <= 0) {
						SIM_TextTime = 20;
						SendMessage(board, 'ToF');
					}
					SIM_TextTime--;
				}
			}
			if (acc.written()) {
				if (acc.value()) {  // any value other than 0
					Serial.print(IoT.localName());
					Serial.println(" event: ");
					Serial.println("acc");
					if (SIM_TextTime <= 0) {
						SIM_TextTime = 20;
						SendMessage(board, 'accelerometer');
					}
					SIM_TextTime--;
				}
			}
			if (gyro.written()) {
				if (gyro.value()) {  // any value other than 0
					Serial.print(IoT.localName());
					Serial.println(" event: ");
					Serial.println("gyro");
					if (SIM_TextTime <= 0) {
						SIM_TextTime = 20;
						SendMessage(board, 'gyroscope');
					}
					SIM_TextTime--;
				}
			}
		}
	}
		// when the central disconnects, print it out:
		// Serial.print(F("Disconnected from peripheral: "));
		// Serial.println(peripheral.address());
		IoT.disconnect();
}

void setup() {
	Serial.begin(9600);
	while (!Serial)
		delay(1);
	BLE.begin();
	SIM7600CE_Setup();  // GSM & GPRS startup
	FingerprintSetup();
	BLERecieverSetup();
	digitalWrite(BUZZER_PIN, LOW);
}

void loop() {
	Locker();
	if (isLocked) {
		Serial.println("Standby");
		BLEReciever();
	}
	else
		Serial.println("Bike riding");
	delay(500);
}