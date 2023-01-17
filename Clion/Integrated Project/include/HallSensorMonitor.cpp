//
// Created by TedWu on 2022/3/6.
//

#include "Arduino.h"
#include "HallSensorMonitor.h"

double HallSensorMonitor::Monitor () {
	/*
	// Monitor the Hall sensor voltage
	*
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
	if(ReadHallSensorVoltage() <= 2)
		CurrentState = false; // False for down state for Hall Sensor
	else
		CurrentState = true;
	if(CurrentState&&Prev_state)
		HallSafeCount -= 0.1;
	else
		HallSafeCount ++;
	return HallSafeCount;
}

double HallSensorMonitor::ReadHallSensorVoltage () {
	double HallSensorVoltage = analogRead(A0) * (5.0 / 1023.0);
	return HallSensorVoltage;
}