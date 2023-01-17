//
// Created by TedWu on 2021/11/30.
//
#include "Arduino.h"
#include "Throttle.h"

double Throttle::ReadThrottleVoltage() {
	throttle_voltage = analogRead(InputPin) * (5.0 / 1023.0) ;
	return throttle_voltage;
}

void Throttle::isMoved() {
	isMovedStatus = true;
}

void Throttle::ThrottleSecureMonitor() {
	// Judge whether the wheel is moved
	if (isMovedStatus) {
		analogWrite(OutputPin, 0);
		isMovedStatus = false;
	}
	else
		analogWrite(OutputPin, int(ReadThrottleVoltage() * 51.2));
}