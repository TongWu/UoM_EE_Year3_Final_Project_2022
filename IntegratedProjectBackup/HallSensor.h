//
// Created by TedWu on 2021/11/29.
//

#ifndef INTEGRATEDPROJECT_HALLSENSOR_H
#define INTEGRATEDPROJECT_HALLSENSOR_H
#include "Arduino.h"
#include "Throttle.h"

class HallSensor {
private:
	double RealHallSensorVoltage = 0;
	int HallSensorStatus = 2;
	int Prev_HallSensorStatus = 2;
	int WheelMoveCount = 0;
	int WheelSleepCount = 0;
	char InputPin;
	bool isMoved = false;
	Throttle *th;

public:
	HallSensor(char READ_HALL_PIN) {this->InputPin = READ_HALL_PIN;}     // Constructor
	HallSensor(char READ_HALL_PIN, Throttle &s) {
		this->InputPin = READ_HALL_PIN;
		th = &s;
	}                                                                   // Override Constructor

	void HallSensorVoltageMonitor();
	double ReadHallSensorVoltage() const;
	bool isStartup();
};

#endif //INTEGRATEDPROJECT_HALLSENSOR_H
