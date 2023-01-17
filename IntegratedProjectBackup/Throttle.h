//
// Created by TedWu on 2021/11/29.
//
#include "Arduino.h"

#ifndef INTEGRATEDPROJECT_THROTTLE_H
#define INTEGRATEDPROJECT_THROTTLE_H

class Throttle {
private:
	char InputPin = 0;
	int OutputPin = 0;
	double throttle_voltage = 0;
	bool isMovedStatus = false;
public:
	explicit Throttle(char ThrottlePin) {this->InputPin = ThrottlePin;}      // Constructor
	Throttle(char ThrottlePin, int OutputPin) {
		this->InputPin = ThrottlePin;
		this->OutputPin = OutputPin;
	}                                                                       // Over-ride Constructor

	double ReadThrottleVoltage();
	void isMoved();
	void ThrottleSecureMonitor();
};

#endif //INTEGRATEDPROJECT_THROTTLE_H
