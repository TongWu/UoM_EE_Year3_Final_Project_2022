#include <Arduino.h>
#include "DecisionAlgorithm.h"

void setup() {
	Serial.begin(9600);
	pinMode(A1, INPUT);
	pinMode(A2, INPUT);
	while (!Serial)
		delay(1);
	// BLE.begin();
	SIM7600CE_Setup();  // GSM & GPRS startup
	FingerprintSetup();
	// BLERecieverSetup();
	digitalWrite(BUZZER_PIN, LOW);
	vl53_Setup();
	IMUsetup();
	//displaySetup();
	RTCSetup();
	//GPSsetup();
}

void loop() {
	Algorithm();
	/*Locker();
	getSensorData();    // Get sensor data from the main board
	// getHardwareData();  // Get hardware data from the main board
	setFlag();          // set flags according to the sensor and hardware data
	StopBuzzer();
	decisionTree();
	Clear();*/
}