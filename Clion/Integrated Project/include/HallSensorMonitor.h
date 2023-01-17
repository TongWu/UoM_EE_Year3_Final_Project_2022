//
// Created by TedWu on 2022/3/6.
//

#ifndef INTEGRATED_PROJECT_HALLSENSORMONITOR_H
#define INTEGRATED_PROJECT_HALLSENSORMONITOR_H

#include "Arduino.h"

class HallSensorMonitor{
private:
public:
	double HallSafeCount = 0;
	bool Prev_state = false;
	HallSensorMonitor() = default;
	double Monitor();
	double ReadHallSensorVoltage();
};

#endif //INTEGRATED_PROJECT_HALLSENSORMONITOR_H
