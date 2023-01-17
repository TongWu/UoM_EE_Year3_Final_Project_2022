//
// Created by TedWu on 2022/3/30.
//

#ifndef MAINBOARDWIRED_DECISIONALGORITHM_H
#define MAINBOARDWIRED_DECISIONALGORITHM_H

#include "Arduino.h"
#include "buzzer.h"
#include "Fingerprint.h"
#include "VL53L1X.h"
#include "HallSensor.h"
#include "Throttle.h"
#include "SIM7600CE.h"
//#include "SIM7000C.h"
#include "RTC.h"
#include "EulerAngle.h"
#include "GPS.h"
#include "Display.h"


#define ROLL_THRESHOLD 3
#define PITCH_THRESHOLD 3
#define TOF_THRESHOLD 5
#define SENSOR_ROLLING 15
#define SIM_LIMIT_TIME 200
#define DECISION_ROLLING 10

bool eventFlag[7] = {false, false, false, false, false, false, false};
/*
 * Table of eventFlag
 * 0. Fingerprint
 * 1. Hall Sensor
 * 2. Throttle Voltage
 * 3. ToF value
 * 4. Roll
 * 5. Pitch
 */
int SIM_TextTime = 0;
int ToF_Count = 0;
int rollCount = 0;
int pitchCount = 0;
int displayCount = 0;
float distance = 0;

/*
 * getSensorData();
 * input -> void
 * output-> void
 * Function:
 * Get the time-of-flight sensor data
 * Get the IMU data, convert to Euler Angle
 */
void getSensorData()
{
	//LSM6DS33();
	// setPrevious();
	/*for (int i = 0; i < 15; ++i) {
	  LSM6DS33();
	  distance += getDistance() / 10.0;
	  accValue += AccelCalculateDifference();
	  gyroValue += getGyro();
	}
	gyroValue = gyroValue / 15;
	distance = distance / 15;
	accValue = accValue / 15;*/

	/*Get distance*/
	for (int i = 0; i < SENSOR_ROLLING; ++i) {
		distance += getDistance() / 10.0;
	}
	distance = distance / SENSOR_ROLLING;
	/*Get Euler Angle*/
	getEulerAngle();
	/*Print them on the IDE*/
	/*
	Serial.print("ToF: ");
	Serial.println(distance);
	Serial.print("Roll: ");
	Serial.println(complementaryFilter_roll);
	Serial.print("Pitch: ");
	Serial.println(complementaryFilter_pitch);
	*/
	/*Print them on the display*/
	/*if (displayCount == 0) {
		display.clearDisplay();
		display.setCursor(0,0);
		display.print("ToF\nRoll\nPitch");
	  display.setCursor(30,0);
	  display.print(distance);
	  display.setCursor(30,8);
	  display.print(complementaryRoll);
	  display.setCursor(30,16);
	  display.print(complementaryPitch);
	  display.display();
		displayCount = 5;
	} else {displayCount--;}*/
}

/*
 * setFlag();
 * input -> void
 * output-> void
 * Function:
 *
 */
void setFlag() {
	/*Fingerprint*/
	if (isLocked)
		eventFlag[0] = true;
	else
		eventFlag[0] = false;

	/*Hall Sensor*/
	/*
	bool CurrentHall, PrevHall;
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
	}
	*/

	/*Throttle*/
	/*
	if (HardwareData[0] >= 1.8)
	  eventFlag[2] = true;
	*/

	/*Proximity*/
	if (distance <= TOF_THRESHOLD && distance >= 0)
		eventFlag[3] = true;
	else if (distance > TOF_THRESHOLD)
		eventFlag[3] = false;

	/*Roll*/
	if (abs((int)(diff_complementaryRoll)) >= ROLL_THRESHOLD) {
		eventFlag[4] = true;
	} else {
		eventFlag[4] = false;
	}

	/*Pitch*/
	if (abs((int)(diff_complementaryPitch)) >= PITCH_THRESHOLD)
		eventFlag[5] = true;
	else {
		eventFlag[5] = false;
	}
}

/*
 * MainBoardTheft();
 * input -> void
 * output-> void
 * Function:
 *
 */
void MainBoardTheft(int charEventFlag) {
	StartBuzzer();
	//getGPS();
	// Serial.println(AlarmFlag[pointer]);
	Serial.print("Main Board: ");
	switch (charEventFlag) {
		case 1:
			Serial.println("Hall Sensor");
			SendMessage(0, 1);
			break;
		case 2:
			Serial.println("Throttle");
			SendMessage(0, 2);
			break;
		case 3:
			Serial.println("Hall Sensor & Throttle");
			SendMessage(0, 3);
			break;
		case 4:
			Serial.println("Proximity & Roll & Pitch");
			SendMessage(0, 4);
			break;
		case 5:
			Serial.println("Proximity & Roll");
			SendMessage(0, 5);
			break;
		case 6:
			Serial.println("Proximity & Pitch");
			SendMessage(0, 6);
			break;
		case 7:
			Serial.println("Roll & Pitch");
			SendMessage(0, 7);
			break;
		case 8:
			Serial.println("Proximity");
			SendMessage(0, 8);
			break;
		case 9:
			Serial.println("Roll");
			SendMessage(0, 9);
			break;
		case 10:
			Serial.println("Pitch");
			SendMessage(0, 10);
			break;
	}
	//printGPS();
	// Serial.println(charEventFlag);
	if (SIM_TextTime == 0) {
		SIM_TextTime = SIM_LIMIT_TIME;
		// SendMessage('Main Board', charEventFlag);
	}
}

void decisionTree() {
	if (eventFlag[0]/*Fingerprint*/) {
		if (eventFlag[1]/*Hall Sensor*/) {
			if (eventFlag[2]/*Throttle*/) {
				//Serial.println("Hall sensor & Throttle");
				MainBoardTheft(3);
			} else {
				//Serial.println("Hall sensor");
				MainBoardTheft(1);
			}
		} else if (eventFlag[2]/*Throttle*/) {
			if (eventFlag[1]/*Hall Sensor*/) {
				//Serial.println("Hall sensor & Throttle");
				MainBoardTheft(3);
			} else {
				//Serial.println("Throttle");
				MainBoardTheft(2);
			}
		} else { /*Check all sensor*/
			if (eventFlag[3] /*ToF*/ && eventFlag[4] /*Roll*/ && eventFlag[5]/*Pitch*/) {
				//Serial.println("tof & Roll & Pitch");
				MainBoardTheft(4);
			}
				// Check each sensor
				// check ToF starts
			else if(eventFlag[3]/*ToF*/) { //check each sensor
				if (eventFlag[4]/*Roll*/) {
					if (eventFlag[5]/*Pitch*/) {
						//Serial.println("tof & Roll & Pitch");
						MainBoardTheft(4);
					}
					else {
						//Serial.println("tof & Roll");
						MainBoardTheft(5);
					}
				} else if (eventFlag[5]/*Pitch*/) {
					if (eventFlag[4]/*Roll*/) {
						//Serial.println("tof & Roll & Pitch");
						MainBoardTheft(4);
					} else {
						//Serial.println("tof & Pitch");
						MainBoardTheft(6);
					}
				} else {
					ToF_Count++;
					if (ToF_Count >= DECISION_ROLLING) {
						//Serial.println("tof");
						MainBoardTheft(8);
						ToF_Count = 0;
					}
				}
			}
				// ToF part ends
				// check Roll starts
			else if(eventFlag[4]/*Roll*/) {
				if (eventFlag[3]/*ToF*/) {
					//Serial.println("tof & Roll");
					MainBoardTheft(5);
				} else if (eventFlag[5]/*Pitch*/) {
					//Serial.println("Roll & Pitch");
					MainBoardTheft(7);
				} else {
					rollCount++;
					if (rollCount >= DECISION_ROLLING) {
						//Serial.println("Roll");
						MainBoardTheft(9);
						rollCount = 0;
					}
				}
			}
				// acc part ends
				// check Pitch starts
			else if(eventFlag[5]/*Pitch*/) {
				if (eventFlag[3]/*ToF*/) {
					//Serial.println("tof & Pitch");
					MainBoardTheft(6);
				} else if (eventFlag[4]/*Roll*/) {
					//Serial.println("Roll & Pitch");
					MainBoardTheft(7);
				} else {
					pitchCount++;
					if (pitchCount >= DECISION_ROLLING) {
						//Serial.println("Pitch");
						MainBoardTheft(10);
						pitchCount = 0;
					}
				}
			}
		}
	} else {
		Serial.println("Bike riding");
		/*if (displayCount == 0) {
		  display.setCursor(40,32);
		  display.print("Bike riding");
		  display.display();
		}*/
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
}

void readSupplementaryBoardTheft(int supplementaryBoard) {
	float supplementaryVoltage = 0;
	switch (supplementaryBoard) {
		case 1:
			supplementaryVoltage = analogRead(A1) / 4;
			break;
		case 2:
			supplementaryVoltage = analogRead(A2) / 4;
			break;
	}
	int supplementaryEvent = 0;
	//Serial.println(supplementaryVoltage);
	if (supplementaryVoltage >= 15) { /*If the supplementary voltage is non-zero*/

		/*Start classify the voltage*/
		if ( supplementaryVoltage >= 20  && supplementaryVoltage <= 25  ) {
			supplementaryEvent = 1;
		}
		else if(supplementaryVoltage >= 30  && supplementaryVoltage <= 35) {
			supplementaryEvent = 2;
		}
		else if(supplementaryVoltage >= 41  && supplementaryVoltage <= 49) {
			supplementaryEvent = 3;
		}
		else if(supplementaryVoltage >= 55  && supplementaryVoltage <= 60) {
			supplementaryEvent = 4;
		}
		else if(supplementaryVoltage >= 69  && supplementaryVoltage <= 74) {
			supplementaryEvent = 5;
		}
		else if(supplementaryVoltage >= 82  && supplementaryVoltage <= 87) {
			supplementaryEvent = 6;
		}
		else if(supplementaryVoltage >= 95  && supplementaryVoltage <= 100) {
			supplementaryEvent = 7;
		}

		/*Classify the Event and send message*/
		Serial.print("Supplementary board ");
		Serial.print(supplementaryBoard);
		switch (supplementaryEvent) {
			case 1:
				Serial.println("Proximity & Roll & Pitch");
				break;
			case 2:
				Serial.println("Proximity & Roll");
				break;
			case 3:
				Serial.println("Proximity & Pitch");
				break;
			case 4:
				Serial.println("Roll & Pitch");
				break;
			case 5:
				Serial.println("Proximity");
				break;
			case 6:
				Serial.println("Roll");
				break;
			case 7:
				Serial.println("Pitch");
				break;
		}
	}
}

void Algorithm() {
    //int start = millis();
    Locker();
	getSensorData();    // Get sensor data from the main board
    printCalculations();
    Serial.print("Distance: ");
    Serial.println(distance);
	// getHardwareData();  // Get hardware data from the main board
	setFlag();          // set flags according to the sensor and hardware data
	StopBuzzer();
	decisionTree();
	readSupplementaryBoardTheft(1);
	Clear();
    //int gap = millis() - start;
    //Serial.println(gap);
	//getGPS();
}

#endif //MAINBOARDWIRED_DECISIONALGORITHM_H
