#include <Arduino.h>
#include "buzzer.h"
#include "SIM7600CE.h"
#include "Fingerprint.h"
#include "LSM6DS33.h"
#include "VL53L1X.h"
#include "Adafruit_SH110X.h"
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
Adafruit_SH1107 display = Adafruit_SH1107(64, 128, &Wire);

bool eventFlag[7] = {false};
/*
 * Table of eventFlag
 * 1. Hall Sensor
 * 2. Throttle Voltage
 * 3. Gyroscope Offset
 * 4. Accelerometer Offset
 * 5. Proximity Value
 * 6. Fingerprint
 */
int SIM_TextTime = 0;
int ToF_Count = 0;
int accCount = 0;
int gyroCount = 0;
float distance = 0;
float accValue = 0;
float gyroValue = 0;

/*void BLERecieverSetup() {
	// begin initialization
	if (!BLE.begin()) {
		Serial.println("starting Bluetooth® Low Energy module failed!");

		while (1);
	}
	// set advertised local name and service UUID:
	BLE.setLocalName("Central");
	BLE.setAdvertisedService(Supplementary);
	// add the characteristic to the service
	Supplementary.addCharacteristic(proximityCharacteristic);
	Supplementary.addCharacteristic(accCharacteristic);
	Supplementary.addCharacteristic(gyroCharacteristic);
	// add service
	BLE.addService(Supplementary);
	// set the initial value for the characeristic:
	proximityCharacteristic.writeValue(0);
	accCharacteristic.writeValue(0);
	gyroCharacteristic.writeValue(0);
	// start advertising
	BLE.advertise();
	Serial.println("BLE Central");
}*/

/*void BLEReciever() {
	BLEDevice peripheral = BLE.central();
	int board = 0;
	if (peripheral) {
		Serial.print("Connected to central: ");
		// print the central's MAC address:
		Serial.println(peripheral.address());
		if (peripheral.localName() == "Frame")
			board = 1;
		else if (peripheral.localName() == "Front")
			board = 2;
		else if (peripheral.localName() == "Back")
			board = 3;
		else
			board = 0;
		// while the central is still connected to peripheral:
		if (peripheral.connected()) {
			// if the remote device wrote to the characteristic,
			// use the value to control the LED:
			if (proximityCharacteristic.written()) {
				if (proximityCharacteristic.value()) {  // any value other than 0
					Serial.println("Proximity");
					switch (board) {
						case 1:
							Supplementary1Event[0] = true;
							break;
						case 2:
							Supplementary2Event[0] = true;
							break;
						case 3:
							Supplementary3Event[0] = true;
					}
				}
			}
			if (accCharacteristic.written()) {
				if (accCharacteristic.value()) {
					Serial.println("Accelerometer");
					switch (board) {
						case 1:
							Supplementary1Event[1] = true;
							break;
						case 2:
							Supplementary2Event[1] = true;
							break;
						case 3:
							Supplementary3Event[1] = true;
					}
				}
			}
			if (gyroCharacteristic.written()) {
				if (gyroCharacteristic.value()) {
					Serial.println("Gyro");
					switch (board) {
						case 1:
							Supplementary1Event[2] = true;
							break;
						case 2:
							Supplementary2Event[2] = true;
							break;
						case 3:
							Supplementary3Event[2] = true;
					}
				}
			}
		}
		// when the central disconnects, print it out:
		// Serial.print(F("Disconnected from peripheral: "));
		// Serial.println(peripheral.address());
	}
	peripheral.disconnect();
}*/

void getSensorData() {
	LSM6DS33();
	// setPrevious();
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
	display.setCursor(28,0);
	display.print(distance);
	display.setCursor(28,8);
	display.print(accValue);
	display.setCursor(28,0);
	display.print(gyroValue);
	display.display();
}

void SetFlag() {
	/*Fingerprint*/
	if (isLocked)
		eventFlag[0] = true;
	else
		eventFlag[0] = false;

	/*Hall Sensor*/
	/*bool CurrentHall, PrevHall;
	if (HardwareData[1] <= 2)
		CurrentHall = false;
	else if (HardwareData[1] >= 4)
		CurrentHall = true;
	if (HardwareData[2] <= 2)
		PrevHall = false;
	else if (HardwareData[2] >=4)
		PrevHall = true;

	if (CurrentHall != PrevHall) {
		if (HallCount >= 0)
			HallCount++;
	}
	else {
		if (HallCount > 0)
			HallCount -= 0.1;
	}
	if (HallCount >= 15) {
		eventFlag[1] = true;
		HallCount = 0;
	}*/

	/*Throttle*/
	/*if (HardwareData[0] >= 1.8)
		eventFlag[2] = true;*/

	/*Proximity*/
	if (distance <= 5 && distance >= 0)
		eventFlag[5] = true;
	else if (distance > 5)
		eventFlag[5] = false;

	/*Accelerometer*/
	if (accValue >= 2)
		eventFlag[4] = true;
	else {
		eventFlag[4] = false;
	}

	/*Gyro*/
	if (gyroValue >= 2) {
		eventFlag[3] = true;
	} else {
		eventFlag[3] = false;
	}
}

void MainBoardTheft(int chareventFlag) {
	StartBuzzer();
	// Serial.println(AlarmFlag[pointer]);
	Serial.print("Main Board: ");
	switch (chareventFlag) {
		case 1:
			Serial.println('Hall Sensor');
			break;
		case 2:
			Serial.println('Throttle');
			break;
		case 3:
			Serial.println('Hall Sensor & Throttle');
			break;
		case 4:
			Serial.println('Proximity & Accelerometer & Gyroscope');
			break;
		case 5:
			Serial.println('Proximity & Accelerometer');
			break;
		case 6:
			Serial.println('Proximity & Gyroscope');
			break;
		case 7:
			Serial.println('Accelerometer & Gyroscope');
			break;
		case 8:
			Serial.println('Proximity');
			break;
		case 9:
			Serial.println('Accelerometer');
			break;
		case 10:
			Serial.println('Gyroscope');
			break;
	}
	// Serial.println(chareventFlag);
	if (SIM_TextTime == 0) {
		SIM_TextTime = 200;
		// SendMessage('Main Board', chareventFlag);
	}
}

void decisionTree() {
	if (eventFlag[0]/*Fingerprint*/) {
		if (eventFlag[1]/*Hall Sensor*/) {
			if (eventFlag[2]/*Throttle*/) {
				Serial.println("Hall sensor & Throttle");
				MainBoardTheft(3);
			} else {
				Serial.println("Hall sensor");
				MainBoardTheft(1);
			}
		} else if (eventFlag[2]/*Throttle*/) {
			if (eventFlag[1]/*Hall Sensor*/) {
				Serial.println("Hall sensor & Throttle");
				MainBoardTheft(3);
			} else {
				Serial.println("Throttle");
				MainBoardTheft(2);
			}
		} else { /*Check all sensor*/
			if (eventFlag[3] /*ToF*/ && eventFlag[4] /*acc*/ && eventFlag[5]/*gyro*/) {
				Serial.println("tof & acc & gyro");
				MainBoardTheft(4);
			}
			// Check each sensor
			// check ToF starts
			else if(eventFlag[3]/*ToF*/) { //check each sensor
				if (eventFlag[4]/*acc*/) {
					if (eventFlag[5]/*gyro*/) {
						Serial.println("tof & acc & gyro");
						MainBoardTheft(4);
					}
					else {
						Serial.println("tof & acc");
						MainBoardTheft(5);
					}
				} else if (eventFlag[5]/*gyro*/) {
					if (eventFlag[4]/*acc*/) {
						Serial.println("tof & acc & gyro");
						MainBoardTheft(4);
					} else {
						Serial.println("tof & gyro");
						MainBoardTheft(6);
					}
				} else {
					ToF_Count++;
					if (ToF_Count >= 30) {
						Serial.println("tof");
						MainBoardTheft(8);
						ToF_Count = 0;
					}
				}
			}
			// ToF part ends
			// check acc starts
			else if(eventFlag[4]/*acc*/) {
				if (eventFlag[3]/*ToF*/) {
					Serial.println("tof & acc");
					MainBoardTheft(5);
				} else if (eventFlag[5]/*gyro*/) {
					Serial.println("acc & gyro");
					MainBoardTheft(7);
				} else {
					accCount++;
					if (accCount >= 30) {
						Serial.println("acc");
						MainBoardTheft(9);
						accCount = 0;
					}
				}
			}
			// acc part ends
			// check gyro starts
			else if(eventFlag[5]/*gyro*/) {
				if (eventFlag[3]/*ToF*/) {
					Serial.println("tof & gyro");
					MainBoardTheft(6);
				} else if (eventFlag[4]/*acc*/) {
					Serial.println("acc & gyro");
					MainBoardTheft(7);
				} else {
					gyroCount++;
					if (gyroCount >= 30) {
						Serial.println("gyro");
						MainBoardTheft(10);
						gyroCount = 0;
					}
				}
			}
		}
	} else {
		Serial.println("Bike riding");
	}
}

void Clear() {
	// StopBuzzer();
	if(SIM_TextTime > 0)
		SIM_TextTime--;
	// Clear Event Flag
	for (bool & i : eventFlag) {
		i = false;
	}
	/*for (int i = 0; i < 3; ++i) {
		Supplementary1Event[i] = false;
		Supplementary2Event[i] = false;
		Supplementary3Event[i] = false;
	}*/
	distance = 0;
	accValue = 0;
	gyroValue = 0;
}

/*void BLEAlgorithm() {
	// Take sample frequency per 0.5 sec
	Locker();           // Delay 0.24s
	// getSensorData();    // Get sensor data from the main board
	// getHardwareData();  // Get hardware data from the main board
	SetFlag();          // set flags according to the sensor and hardware data
	// First connect to the frame supplementary board
	BLE.scanForUuid("193F");
	Frame = BLE.available();
	while (!Frame) {
		// discovered a peripheral, print out address, local name, and advertised service
		Serial.print("Found ");
		Serial.print(Frame.address());
		Serial.print(" '");
		Serial.print(Frame.localName());
		Serial.print("' ");
		Serial.print(Frame.advertisedServiceUuid());
		Serial.println();
		// stop scanning
		BLE.stopScan();
		BLEReciever(Frame, 1);      // get data and set flag from the supplementary board
		Frame.disconnect();
		// peripheral disconnected, start scanning again
		BLE.scanForUuid("194F");
	}
	Front = BLE.available();
	while (!Front) {
		// discovered a peripheral, print out address, local name, and advertised service
		Serial.print("Found ");
		Serial.print(Front.address());
		Serial.print(" '");
		Serial.print(Front.localName());
		Serial.print("' ");
		Serial.print(Front.advertisedServiceUuid());
		Serial.println();
		// stop scanning
		BLE.stopScan();
		BLEReciever(Front, 2);      // get data and set flag from the supplementary board
		Front.disconnect();
		// peripheral disconnected, start scanning again
		BLE.scanForUuid("195F");
	}
	Front = BLE.available();
	while (Back) {
		// discovered a peripheral, print out address, local name, and advertised service
		Serial.print("Found ");
		Serial.print(Back.address());
		Serial.print(" '");
		Serial.print(Back.localName());
		Serial.print("' ");
		Serial.print(Back.advertisedServiceUuid());
		Serial.println();
		// stop scanning
		BLE.stopScan();
		BLEReciever(Back, 3);      // get data and set flag from the supplementary board
		Back.disconnect();
		// peripheral disconnected, start scanning again
		BLE.scanForUuid("193F");
	}
	BLEReciever();
	// BLEReciever();
	// BLEReciever();
	StopBuzzer();
	// SetPrevData();
	// DecisionTree();     // to decide the theft action (include StartBuzzer())
	Supplementray1DecisionTree();
	// Supplementray2DecisionTree();
	// Supplementray3DecisionTree();
	Clear();
	// Delay(100);      // Delay 0.1s
	// Total delay is bigger than 0.34s
}*/

void Algorithm() {
	// Take sample frequency per 0.5 sec
	Locker();           // Delay 0.24s
	getSensorData();    // Get sensor data from the main board
	// getHardwareData();  // Get hardware data from the main board
	SetFlag();          // set flags according to the sensor and hardware data
	StopBuzzer();
	decisionTree();
	Clear();
}

void setup() {
	Serial.begin(9600);
	while (!Serial)
		delay(1);
	// BLE.begin();
	SIM7600CE_Setup();  // GSM & GPRS startup
	FingerprintSetup();
	// BLERecieverSetup();
	digitalWrite(BUZZER_PIN, LOW);
	vl53_Setup();
	LSM6DS33_Setup();
	LSM6DS33();
	Serial.println("128x64 OLED FeatherWing test");
	display.begin(0x3C, true); // Address 0x3C default
	Serial.println("OLED begun");
	display.display();
	delay(1000);
	display.clearDisplay();
	display.display();
	display.setRotation(1);
	display.setTextSize(1);
	display.setTextColor(SH110X_WHITE);
	display.setCursor(0,0);
	display.print("ToF\nAcc\nGyro");
	display.display(); // actually display all of the above
}

void loop() {
	Algorithm();
	delay(100);
}