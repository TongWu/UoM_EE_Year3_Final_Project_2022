//
// Created by TedWu on 2022/3/6.
//

#ifndef INTEGRATED_PROJECT_THROTTLEMONITOR_H
#define INTEGRATED_PROJECT_THROTTLEMONITOR_H

#include "Arduino.h"

class ThrottleMonitor{
private:
	const int THROTTLE_PIN = 11;
public:
	ThrottleMonitor() = default;
	void Monitor();
	double ThrottleVoltage();
};

#endif //INTEGRATED_PROJECT_THROTTLEMONITOR_H
