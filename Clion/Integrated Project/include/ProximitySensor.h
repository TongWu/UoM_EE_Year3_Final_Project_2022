//
// Created by TedWu on 2022/3/6.
//

#ifndef INTEGRATED_PROJECT_PROXIMITYSENSOR_H
#define INTEGRATED_PROJECT_PROXIMITYSENSOR_H

#include "Arduino.h"
#include "Adafruit_VCNL4040.h"

class VCNL4040{
private:
	// Create the Proximity Sensor class object
	Adafruit_VCNL4040 ProximitySensor = Adafruit_VCNL4040();
	// Initialize global parameters
	uint16_t Prev_LuxValue = 0;
	uint16_t Normal_LuxValue = 0;
	bool Prev_isSafe = false;

public:
	void NormalLuxValueDetect();
	bool LuxSafeStateDetect(uint16_t InputValue);
	bool LuxSafeAlgorithm(uint16_t CurrentLuxValue);
	void AdjustNormalLux(uint16_t CurrentValue);
	bool Algorithm();
	void Setup();

};

#endif //INTEGRATED_PROJECT_PROXIMITYSENSOR_H
