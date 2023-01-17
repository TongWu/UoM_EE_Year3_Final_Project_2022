//
// Created by TedWu on 2021/11/30.
//
#include "Arduino.h"
#include "HallSensor.h"
#include "Throttle.h"

void HallSensor::HallSensorVoltageMonitor() {
	isMoved = false;
	// Monitor the Hall sensor voltage
	// Read hall sensor, classify it as HIGH or LOW
	// If the hall sensor returns 0-2.5V, classify as LOW, classify 2.5-5V to HIGH
	RealHallSensorVoltage = ReadHallSensorVoltage();
	if (RealHallSensorVoltage <=2.5 && RealHallSensorVoltage >=0)
		HallSensorStatus = LOW;
	else if (RealHallSensorVoltage>2.5 && RealHallSensorVoltage <=5)
		HallSensorStatus = HIGH;
	else
		Serial.println("Fail to read wheel status");
	// Judge the Prev_HallSensorStatus
	/*
	 * If the wheel moved, WheelMoveCount++
	 * WheelMoveCount++
	 * If the wheel does not move, WheelSleepCount++
	 */
	if (Prev_HallSensorStatus != HallSensorStatus && Prev_HallSensorStatus != 2) {
		WheelMoveCount++;
		WheelSleepCount = 0;
	}
	else
		WheelSleepCount++;
	// Judge the WheelMoveCount
	/*
	 * If the wheel moves a specific distance which can be classified as anomalous, give a warning, clear its count
	 */
	if (WheelMoveCount >= 100) {
		// Do alarm actions
		Serial.println("Wheel Moved");
		// Rise a alarm flag
		th->isMoved();
		WheelMoveCount = 0;
	}
	// Judge the WheelSleepCount
	if (WheelSleepCount >= 2000) {
		WheelMoveCount = 0;
	}
	// Return HallSensorStatus to Prev.
	Prev_HallSensorStatus = HallSensorStatus;
	/*
	 * Check the voltage change with this sequence:
	 * LOW -> HIGH -> LOW -> HIGH -> LOW -> HIGH -> LOW -> HIGH -> LOW -> HIGH
	 */
	/*
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
	}*/
}

double HallSensor::ReadHallSensorVoltage() const{
	double HallSensorVoltage = analogRead(InputPin) * (5.0 / 1023.0);
	return HallSensorVoltage;
}

bool isStartup() {}
