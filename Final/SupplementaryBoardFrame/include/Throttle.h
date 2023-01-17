//
// Created by TedWu on 2022/3/13.
//

#ifndef MAINBOARD_THROTTLE_H
#define MAINBOARD_THROTTLE_H

#include "Arduino.h"
#define THROTTLE_PIN 11

void ThrottleMonitor () {
	int throttle_voltage = analogRead(A0) ;
	// Serial.println(throttle_voltage );
	analogWrite(THROTTLE_PIN, throttle_voltage /4 );
}

float getThrottleVoltage() {
	return analogRead(A0) / 4.0;
}

#endif //MAINBOARD_THROTTLE_H
