//
// Created by TedWu on 2022/3/13.
//

#ifndef MAINBOARD_HC_SR04_H
#define MAINBOARD_HC_SR04_H

#include "NewPing.h"
// Hook up HC-SR04 with Trig to Arduino Pin 9, Echo to Arduino pin 10
#define TRIGGER_PIN 9
#define ECHO_PIN 10
// Maximum distance we want to ping for (in centimeters).
#define MAX_DISTANCE 400
// NewPing setup of pins and maximum distance.
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);
float duration, distance;

float HC_SR04()
{
	// Send ping, get distance in cm
	distance = sonar.ping_cm();
	if (distance >= MAX_DISTANCE)
		distance = -1;
	else if (distance <= 1)
		distance = 0;
	return distance;
}

#endif //MAINBOARD_HC_SR04_H
