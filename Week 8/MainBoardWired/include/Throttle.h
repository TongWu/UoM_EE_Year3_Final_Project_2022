//
// Created by TedWu on 2022/3/13.
//

#ifndef MAINBOARD_THROTTLE_H
#define MAINBOARD_THROTTLE_H

#include "Arduino.h"
#include "DecisionAlgorithm.h"
#include "Throttle.h"

#define THROTTLE_PIN 11

double getThrottleVoltage() {
	return analogRead(A0) / 4.0;
}

double ThrottleMonitor () {
	//int throttle_voltage = analogRead(A0) ;
	// Serial.println(throttle_voltage );
	if (eventFlag[0]) {
		analogWrite(THROTTLE_PIN, 0);
	} else {
		analogWrite(THROTTLE_PIN, (int)getThrottleVoltage() );
	}
	return getThrottleVoltage();
}

#endif //MAINBOARD_THROTTLE_H
