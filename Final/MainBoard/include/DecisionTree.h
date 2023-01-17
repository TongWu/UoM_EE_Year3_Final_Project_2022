//
// Created by TedWu on 2022/3/24.
//

#ifndef MAINBOARD_DECISIONTREE_H
#define MAINBOARD_DECISIONTREE_H

#include "Arduino.h"

bool eventFlag[7] = {false};
/*
 * Table of EventFlag
 * 0. Fingerprint
 * 1. Hall Sensor
 * 2. Throttle Voltage
 * 3. Proximity Value
 * 4. Accelerometer Offset
 * 5. Gyroscope Offset
 */

void decisionTree() {
	if (eventFlag[0]/*Fingerprint*/) {
		if (eventFlag[1]/*Hall Sensor*/) {
			MainBoardTheft(1);
		} else if (eventFlag[2]/*Throttle*/) {
			MainBoardTheft(2);
		} else { /*Check all sensor*/
			if (eventFlag[3] /*ToF*/ && eventFlag[4] /*acc*/ && eventFlag[5]/*gyro*/) {
				Serial.println("tof & acc & gyro");
			}
			// Check each sensor
			// check ToF start
			else if(eventFlag[3]/*ToF*/) { //check each sensor
				if (eventFlag[4]/*acc*/) {
					Serial.println("tof & acc");
				} else if (eventFlag[5]/*gyro*/) {
					Serial.println("tof & gyro");
				} else {
					ToF_Count++;
					if (ToF_Count >= 30) {
						Serial.println("tof");
						ToF_Count = 0;
					}
				}
			}
			// ToF part ends
			// check acc start
			else if(eventFlag[4]/*acc*/) {
				if (eventFlag[3]/*ToF*/) {
					Serial.println("tof & acc");
				} else if (eventFlag[5]/*gyro*/) {
					Serial.println("acc & gyro");
				} else {
					accCount++;
					if (accCount >= 30) {
						Serial.println("acc");
						accCount = 0;
					}
				}
			}
			else if(eventFlag[5]/*gyro*/) {
				if (eventFlag[3]/*ToF*/) {
					Serial.println("tof & gyro");
				} else if (eventFlag[4]/*acc*/) {
					Serial.println("acc & gyro");
				} else {
					gyroCount++;
					if (gyroCount >= 30) {
						Serial.println("gyro");
						gyroCount = 0;
					}
				}
			}
		}
	}

}

#endif //MAINBOARD_DECISIONTREE_H
