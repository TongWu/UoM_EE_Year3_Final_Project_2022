//
// Created by TedWu on 2022/3/13.
//

#ifndef MAINBOARD_HALL_SENSOR_H
#define MAINBOARD_HALL_SENSOR_H

#include "Arduino.h"
float HallSafeCount = 10;
bool Prev_State = false;
double getHallSensorVoltage() {
	double voltage = analogRead(A1) * (5.0 / 1023.0);
	return voltage;
}

double HallSensorMonitor() {
	/*
	// Monitor the Hall sensor voltage
	 * Check the voltage change with this sequence:
     * LOW -> HIGH -> LOW -> HIGH -> LOW -> HIGH -> LOW -> HIGH -> LOW -> HIGH
	 *
	while(ReadHallSensorVoltage() <= 2) {
		while (ReadHallSensorVoltage() >= 4)
			while (ReadHallSensorVoltage() <= 2)
				while (ReadHallSensorVoltage() >= 4)
					while (ReadHallSensorVoltage() <= 2)
						while (ReadHallSensorVoltage() >= 4)
							while (ReadHallSensorVoltage() <= 2)
								while (ReadHallSensorVoltage() >= 4)
									while (ReadHallSensorVoltage() <= 2)
										while (ReadHallSensorVoltage() >= 4) {
											Serial.println("Wheel Moved");
											break;
										}
	}
	*/
	// Determine the motion state
	bool CurrentState;
	if(getHallSensorVoltage() <= 2) {
		CurrentState = false; // False for down state for Hall Sensor
	}
	else {
		CurrentState = true;
	}

	if(CurrentState & Prev_State) {
		if (HallSafeCount >= 0.5) {
			HallSafeCount -= 0.5;
		}
	}
	else {
		if (HallSafeCount <= 9) {
			HallSafeCount ++;
		}
	}
	return HallSafeCount;
}

#endif //MAINBOARD_HALL_SENSOR_H
