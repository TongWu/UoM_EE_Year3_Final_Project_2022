//
// Created by TedWu on 2022/3/15.
//

#ifndef MAINBOARD_VL53L1X_H
#define MAINBOARD_VL53L1X_H

#include "Arduino.h"
#include "Adafruit_VL53L1X.h"

#define IRQ_PIN 2
#define XSHUT_PIN 3

Adafruit_VL53L1X vl53 = Adafruit_VL53L1X(XSHUT_PIN, IRQ_PIN);

void vl53_Setup() {
	Wire.begin();
	Serial.println("***Initialize the ToF Sensor***");
	while (! vl53.begin(0x29, &Wire)) {
		Serial.print(F("Error on init of VL sensor: "));
		Serial.println(vl53.vl_status);
		delay(10);
	}
	Serial.println(F("VL53L1X sensor OK!"));

	Serial.print(F("VL53L1X Sensor ID: 0x"));
	Serial.println(vl53.sensorID(), HEX);

	while (! vl53.startRanging()) {
		Serial.print(F("Couldn't start ranging: "));
		Serial.println(vl53.vl_status);
		delay(10);
	}
	Serial.println(F("Ranging started"));

	// Valid timing budgets: 15, 20, 33, 50, 100, 200 and 500ms!
	vl53.setTimingBudget(50);
	Serial.print(F("Timing budget (ms): "));
	Serial.println(vl53.getTimingBudget());

	/*
	vl.VL53L1X_SetDistanceThreshold(100, 300, 3, 1);
	vl.VL53L1X_SetInterruptPolarity(0);
	*/

	while(! vl53.dataReady());
	Serial.println("ToF Sensor Ready");

	Serial.println("***End of ToF Sensor***");
}

int16_t getDistance() {
	int16_t distance = vl53.distance();
	// Distance in mm
	if (distance == -1) {
		// something went wrong!
		Serial.print(F("Couldn't get distance: "));
		Serial.println(vl53.vl_status);
	}
	return distance;
}


#endif //MAINBOARD_VL53L1X_H
