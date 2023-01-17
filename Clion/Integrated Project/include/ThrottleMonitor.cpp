//
// Created by TedWu on 2022/3/6.
//

#include "ThrottleMonitor.h"

void ThrottleMonitor::Monitor () {
	int throttle_voltage = analogRead(A0) ;
	Serial.println(throttle_voltage );
	analogWrite(THROTTLE_PIN, throttle_voltage /4 );
}

double ThrottleMonitor::ThrottleVoltage() {
	return analogRead(A0) / 4.0;
}