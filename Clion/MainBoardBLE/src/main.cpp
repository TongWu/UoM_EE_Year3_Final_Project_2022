#include "Arduino.h"
#include "Hall Sensor.h"
#include "LSM6DS33.h"
#include "buzzer.h"
#include "Throttle.h"
#include "SIM7600CE.h"
#include "VL53L1X.h"
#include "Fingerprint.h"
#include "BLEReciever.h"

bool EventFlag[7] = {false};
/*
 * Table of EventFlag
 * 1. Hall Sensor
 * 2. Throttle Voltage
 * 3. Gyroscope Offset
 * 4. Accelerometer Offset
 * 5. Proximity Value
 * 6. Fingerprint
 */
int SIM_TextTime = 0;
/*char AlarmFlag[8] = {
		'zero',
		'Hall Sensor Unsafe',
		'Throttle Unsafe',
		'Gyroscope Unsafe',
		'Accelerometer Unsafe',
		'Proximity Unsafe',
		'All Sensor Unsafe',
		'Gyroscope and Accelerometer Unsafe'
};*/

float SensorData[6] = {
		0,  // Proximity Current Value (ultrasound OR ToF OR APDS9960 OR VCNL4040)
		0,  // Proximity Previous Value
		0,  // Accelerometer Current Value
		0,  // Accelerometer Prev Value
		0,  // Gyro Current Value
		0   // Gyro Previous Value
};

float HardwareData[3] = {
		0,  // Throttle Value
		0,  // Hall Sensor Current Value
		0,  // Hall Sensor Previous Value
};

bool Supplementary2Event[3] = {
		false, // 2: proximity
		false, // 2: acc
		false, // 2: gyro
};

float HallCount;

void getSensorData() {
	/*Proximity Sensor (UltraSound)*/
	// SensorData[0] = HC_SR04();
	/*Print Proximity Value*/
	// Serial.print("Proximity: ");
	// Serial.print(ProximityDistance);
	// Serial.println(" cm");

	/*ToF Sensor*/
	SensorData[0] = getDistance() / 10.0; // in cm


	/*Accelerometer & Gyroscope*/
	LSM6DS33();
	SensorData[2] = AccelCalculateDifference();
	SensorData[4] = getGyro();

}

void getHardwareData() {
	/*Throttle*/
	HardwareData[0] = getThrottleVoltage();

	/*Hall Sensor*/
	HardwareData[1] = getHallSensorVoltage();
}

void SetFlag() {
	/*Fingerprint*/
	if (isLocked)
		EventFlag[6] = true;
	else
		EventFlag[6] = false;

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
		EventFlag[1] = true;
		HallCount = 0;
	}*/

	/*Throttle*/
	/*if (HardwareData[0] >= 1.8)
		EventFlag[2] = true;*/

	/*Proximity*/
	if (SensorData[0] <= 5 && SensorData[0] >= 0)
		EventFlag[5] = true;
	else if (SensorData[0] > 5)
		EventFlag[5] = false;

	/*Accelerometer*/
	if (SensorData[2] >= 2)
		EventFlag[4] = true;

	/*Gyro*/
	if (SensorData[4] >=2)
		EventFlag[3] = true;
}

void MainBoardTheft(int charEventFlag) {
	StartBuzzer();
	// Serial.println(AlarmFlag[pointer]);
	Serial.print("Main Board: ");
	switch (charEventFlag) {
		case 1:
			Serial.println('Hall Sensor');
			break;
		case 2:
			Serial.println('Throttle');
			break;
		case 3:
			Serial.println('Proximity & Accelerometer & Gyroscope');
			break;
		case 4:
			Serial.println('Accelerometer & Gyro');
			break;
		case 5:
			Serial.println('Proximity');
			break;
	}
	// Serial.println(charEventFlag);
	if (SIM_TextTime == 0) {
		SIM_TextTime = 200;
		// SendMessage('Main Board', charEventFlag);
	}
}

void SupplementaryBoardTheft(int charEventFlag) {
	StartBuzzer();
	Serial.print("Supplementary Board: ");
	switch (charEventFlag) {
		case 1:
			Serial.println('Hall Sensor');
			break;
		case 2:
			Serial.println('Throttle');
			break;
		case 3:
			Serial.println('Proximity & Accelerometer & Gyroscope');
			break;
		case 4:
			Serial.println('Accelerometer & Gyro');
			break;
		case 5:
			Serial.println('Proximity');
			break;
	}
	// Serial.println(AlarmFlag[pointer]);
	if (SIM_TextTime == 0) {
		SIM_TextTime = 200;
		// SendMessage('Supplementary Board', charEventFlag);
	}
}

void DecisionTree() {
	// EventFlag[6] = true; // Fingerprint is Locked as default
	// Decision Tree
	if (EventFlag[6]) {
		/*Print sensor values*/
		/*Serial.print("ToF: ");
		Serial.println(SensorData[0]);
		Serial.print("Gyroscope: ");
		Serial.print(SensorData[4]);
		Serial.print("\t Accelerometer: ");
		Serial.println(SensorData[2]);*/
		// If Fingerprint is unlocked
		// Check Speed (Hall Sensor, GPS)
		if (EventFlag[1]) {
			MainBoardTheft(1);
		}
		else if (EventFlag[2])
			MainBoardTheft(2);
			// If Speed Safe
			// Check Throttle
		else {
			// if Throttle safe
			// Check All Sensor
			if (EventFlag[3] && EventFlag[4] && EventFlag[5])
				// If all sensor unsafe
				MainBoardTheft(3);
			else {
				if (EventFlag[3] || EventFlag[4]) {
					// If accelerometer or gyroscope unsafe
					// Check the proximity if it is unsafe
					if (EventFlag[5])
						MainBoardTheft(3);
					else {
						for (int i = 0; i < 20;) {
							if (EventFlag[3] && EventFlag[4] && EventFlag[5]) {
								MainBoardTheft(3);
								break;
							}
							else if (EventFlag[3] || EventFlag[4]) {
								if (EventFlag[5]) {
									MainBoardTheft(3);
									break;
								}
								else
									i++;
							}
							else if (EventFlag[5]) {
								if (EventFlag[3] || EventFlag[4]) {
									MainBoardTheft(3);
									break;
								}
								else
									i++;
							}
							else
								i--;
							if (i == 20) {
								MainBoardTheft(4);
							}
							delay(25);
						}
					}
				}
				else if (EventFlag[5]) {
					if (EventFlag[3] || EventFlag[4])
						MainBoardTheft(3);
					else {
						for (int i = 0; i < 20;) {
							if (EventFlag[3] && EventFlag[4] && EventFlag[5]) {
								MainBoardTheft(3);
								break;
							}
							else if (EventFlag[3] || EventFlag[4]) {
								if (EventFlag[5]) {
									MainBoardTheft(3);
									break;
								}
								else
									i++;
							}
							else if (EventFlag[5]) {
								if (EventFlag[3] || EventFlag[4]) {
									MainBoardTheft(3);
									break;
								}
								else
									i++;
							}
							else
								i--;
							if (i == 20) {
								MainBoardTheft(5);
							}
							delay(25);
						}
					}
				}
			}
		}
	}
	else {
		Serial.println("Bike riding");
	}
}

void SupplementrayDecisionTree() {
	if (EventFlag[6]) {
		// If proximity sensor is not safe
		if (Supplementary1Event[0] && Supplementary1Event[1] && Supplementary1Event[2]) {
			// if all sensor not safe
			SupplementaryBoardTheft(3);
		} else{
			if (Supplementary1Event[0]) {
				if (Supplementary1Event[1] || Supplementary1Event[2])
					SupplementaryBoardTheft(3);
				else {
					for (int i = 0; i < 20;) {
						if (Supplementary1Event[0] && Supplementary1Event[1] && Supplementary1Event[2]) {
							SupplementaryBoardTheft(3);
							break;
						}
						else if (Supplementary1Event[1] || Supplementary1Event[2]) {
							if (Supplementary1Event[0]) {
								SupplementaryBoardTheft(3);
								break;
							}
							else
								i++;
						}
						else if (Supplementary1Event[0]) {
							if (Supplementary1Event[1] || Supplementary1Event[2]) {
								SupplementaryBoardTheft(3);
								break;
							}
							else
								i++;
						}
						else
							i--;
						if (i == 20) {
							SupplementaryBoardTheft(5);
						}
						delay(25);
					}
				}
			}
			else if (Supplementary1Event[1] || Supplementary1Event[2]) {
				// If accelerometer or gyroscope unsafe
				// Check the proximity if it is unsafe
				if (Supplementary1Event[0])
					SupplementaryBoardTheft(3);
				else {
					for (int i = 0; i < 20;) {
						if (Supplementary1Event[0] && Supplementary1Event[1] && Supplementary1Event[2]) {
							SupplementaryBoardTheft(3);
							break;
						}
						else if (Supplementary1Event[1] || Supplementary1Event[2]) {
							if (Supplementary1Event[0]) {
								SupplementaryBoardTheft(3);
								break;
							}
							else
								i++;
						}
						else if (Supplementary1Event[0]) {
							if (Supplementary1Event[1] || Supplementary1Event[2]) {
								SupplementaryBoardTheft(3);
								break;
							}
							else
								i++;
						}
						else
							i--;
						if (i == 20) {
							SupplementaryBoardTheft(4);
						}
						delay(25);
					}
				}
			}
		}
	}
}

void Clear() {
	// StopBuzzer();
	if(SIM_TextTime > 0)
		SIM_TextTime--;
	// Clear Event Flag
	for (int i = 0; i < 6; ++i) {
		EventFlag[i] = false;
	}
}

void Algorithm() {
	// Take sample frequency per 0.5 sec
	Locker();           // Delay 0.24s
	getSensorData();    // Get sensor data from the main board
	getHardwareData();  // Get hardware data from the main board
	SetFlag();          // set flags according to the sensor and hardware data
	BLEReciever();      // get data and set flag from the supplementary board
	StopBuzzer();
	// SetPrevData();
	DecisionTree();     // to decide the theft action (include StartBuzzer())
	SupplementrayDecisionTree();
	Clear();
	// Delay(100);      // Delay 0.1s
	// Total delay is bigger than 0.34s
}

void setup() {
	Serial.begin(9600);
	while (!Serial)
		delay(1);
	// APDS_Setup();
	vl53_Setup();       // ToF startup
	LSM6DS33_Setup();   // Acc & Gyro startup
	SIM7600CE_Setup();  // GSM & GPRS startup
	pinMode(3, OUTPUT); // Ultrasound startup
	FingerprintSetup();
	BLERecieverSetup();
	digitalWrite(BUZZER_PIN, LOW);
	// delay(1000);        // Delay 5 sec to wait sensors
}

void loop() {
	Algorithm();
	delay(100);
}


