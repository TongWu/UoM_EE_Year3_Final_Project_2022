//
// Created by TedWu on 2022/3/30.
//

#include "Arduino.h"
#include "VL53L1X.h"
#include "EulerAngle.h"
//#include "Display.h"
#include "Wire.h"
#include "JY901.h"

#define ROLL_THRESHOLD 8
#define PITCH_THRESHOLD 8
#define TOF_THRESHOLD 5
#define SENSOR_ROLLING 15
#define DECISION_ROLLING 5
#define OUTPUT_PIN 3

bool eventFlag[3] = {false, false, false};
/*
 * Table of eventFlag
 * 0. ToF value
 * 1. Roll
 * 2. Pitch
 */
int ToF_Count = 0;
int rollCount = 0;
int pitchCount = 0;
int displayCount = 0;
long startTime = 0;
int KFroll_diff = 0;
int KFpitch_diff = 0;
int KFyaw_diff = 0;
int KFroll_pre = 0;
int KFpitch_pre = 0;
int KFyaw_pre = 0;
float distance = 0;

/*Get the sensor data and convert the data from IMU to Euler Angle*/
void getSensorData()
{
	/*Get distance*/
	for (int i = 0; i < SENSOR_ROLLING; ++i) {
		distance += getDistance() / 10.0;
	}
  distance /= SENSOR_ROLLING;
	/*Get Euler Angle*/
	getEulerAngle();
  JY901.GetAngle();
	/*Print them on the IDE*/
  /*
	Serial.print(distance);
  Serial.print(" ");
	Serial.print(complementaryRoll);
  Serial.print(" ");
	Serial.println(complementaryPitch);
 */
 /*Print angle on the display unit*/
 Serial.print("Angle:");Serial.print((float)JY901.stcAngle.Angle[0]/32768*180);Serial.print(" ");Serial.print((float)JY901.stcAngle.Angle[1]/32768*180);Serial.print(" ");Serial.println((float)JY901.stcAngle.Angle[2]/32768*180);
 float KF_pitch = (float)JY901.stcAngle.Angle[0]/32768*180;
 float KF_roll = (float)JY901.stcAngle.Angle[1]/32768*180;
 float KF_yaw = (float)JY901.stcAngle.Angle[2]/32768*180;
 KFpitch_diff = KF_pitch - KFpitch_pre;
 KFroll_diff = KF_roll - KFroll_pre;
 KFyaw_diff = KF_yaw - KFyaw_pre;
 KFpitch_pre = KF_pitch;
 KFroll_pre = KF_roll;
 KFyaw_pre = KF_yaw;
 /*
   display.clearDisplay();
    display.setCursor(0,0);
    display.print("ToF\nRoll\nPitch\nYaw");
    display.setCursor(30,0);
    display.print(distance);
    display.setCursor(30,8);
    display.print(KF_roll);
    display.setCursor(30,16);
    display.print(KF_pitch);
    display.setCursor(30,24);
    display.print(KF_yaw);
    display.display();
    displayCount = 5;
    */
}

void SetFlag() {
	/*Proximity*/
	if (distance <= TOF_THRESHOLD && distance >= 0)
		eventFlag[0] = true;
	else if (distance > TOF_THRESHOLD)
		eventFlag[0] = false;

	/*Roll*/
	if (abs(/*diff_complementaryRoll*/KFroll_diff) >= ROLL_THRESHOLD) {
		eventFlag[1] = true;
	} else {
		eventFlag[1] = false;
	}

	/*Pitch*/
	if (abs(/*diff_complementaryPitch*/KFpitch_diff) >= PITCH_THRESHOLD)
		eventFlag[2] = true;
	else {
		eventFlag[2] = false;
	}
}

void SupplementaryBoardTheft(int EventFlag) {
    //analogWrite(OUTPUT_PIN, 0);
	switch (EventFlag) {
		case 1:
			Serial.println("Proximity & Roll & Pitch");
			analogWrite(OUTPUT_PIN, 40);
			break;
		case 2:
			Serial.println("Proximity & Roll");
			analogWrite(OUTPUT_PIN, 60);
			break;
		case 3:
			Serial.println("Proximity & Pitch");
			analogWrite(OUTPUT_PIN, 80);
			break;
		case 4:
			Serial.println("Roll & Pitch");
			analogWrite(OUTPUT_PIN, 100);
			break;
		case 5:
			Serial.println("Proximity");
			analogWrite(OUTPUT_PIN, 120);
			break;
		case 6:
			Serial.println("Roll");
			analogWrite(OUTPUT_PIN, 140);
			break;
		case 7:
			Serial.println("Pitch");
			analogWrite(OUTPUT_PIN, 160);
			break;
	}
	startTime = millis();
}

void decisionTree() {
	if (eventFlag[0] /*ToF*/ && eventFlag[1] /*Roll*/ && eventFlag[2]/*Pitch*/) {
		//Serial.println("tof & Roll & Pitch");
		SupplementaryBoardTheft(1);
	}
		// Check each sensor
		// check ToF starts
	else if(eventFlag[0]/*ToF*/) { //check each sensor
		if (eventFlag[1]/*Roll*/) {
			if (eventFlag[2]/*Pitch*/) {
				//Serial.println("tof & Roll & Pitch");
				SupplementaryBoardTheft(1);
			}
			else {
				//Serial.println("tof & Roll");
				SupplementaryBoardTheft(2);
			}
		} else if (eventFlag[2]/*Pitch*/) {
			if (eventFlag[1]/*Roll*/) {
				//Serial.println("tof & Roll & Pitch");
				SupplementaryBoardTheft(1);
			} else {
				//Serial.println("tof & Pitch");
				SupplementaryBoardTheft(3);
			}
		} else {
			ToF_Count++;
			if (ToF_Count >= DECISION_ROLLING) {
				//Serial.println("tof");
				SupplementaryBoardTheft(5);
				ToF_Count = 0;
			} else {
				Serial.println("Safe");
				//analogWrite(OUTPUT_PIN, 0);
			}
		}
	}
		// ToF part ends
		// check Roll starts
	else if(eventFlag[1]/*Roll*/) {
		if (eventFlag[0]/*ToF*/) {
			//Serial.println("tof & Roll");
			SupplementaryBoardTheft(2);
		} else if (eventFlag[2]/*Pitch*/) {
			//Serial.println("Roll & Pitch");
			SupplementaryBoardTheft(3);
		} else {
			rollCount++;
			if (rollCount >= 1) {
				//Serial.println("Roll");
				SupplementaryBoardTheft(6);
				rollCount = 0;
			} else {
				Serial.println("Safe");
				//analogWrite(OUTPUT_PIN, 0);
			}
		}
	}
		// acc part ends
		// check Pitch starts
	else if(eventFlag[2]/*Pitch*/) {
		if (eventFlag[0]/*ToF*/) {
			//Serial.println("tof & Pitch");
			SupplementaryBoardTheft(3);
		} else if (eventFlag[1]/*Roll*/) {
			//Serial.println("Roll & Pitch");
			SupplementaryBoardTheft(4);
		} else {
			pitchCount++;
			if (pitchCount >= 1) {
				//Serial.println("Pitch");
				SupplementaryBoardTheft(7);
				pitchCount = 0;
			} else {
				Serial.println("Safe");
				//analogWrite(OUTPUT_PIN, 0);
			}
		}
	}
	else {
		Serial.println("Safe");
		//analogWrite(OUTPUT_PIN, 0);
	}
}

void Clear() {
	// Clear Event Flag
	for (bool & i : eventFlag) {
		i = false;
	}
	distance = 0;
	if ((millis() - startTime) >= 3000) {
		analogWrite(OUTPUT_PIN, 0);
	}
}

void Algorithm() {
	getSensorData();    // Get sensor data from the main board
  printCalculations();
  delay(260);
	// getHardwareData();  // Get hardware data from the main board
	SetFlag();          // set flags according to the sensor and hardware data
	decisionTree();
	Clear();
}
