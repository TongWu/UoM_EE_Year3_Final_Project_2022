//
// Created by TedWu on 2022/3/30.
//

#ifndef MAINBOARDWIRED_RTC_H
#define MAINBOARDWIRED_RTC_H
#include "Arduino.h"
#include "RTClib.h"

RTC_PCF8523 rtc;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

void RTCSetup() {
	while (! rtc.begin()) {
		Serial.println("Couldn't find RTC");
		Serial.flush();
	}
	if (! rtc.initialized() || rtc.lostPower()) {
		Serial.println("RTC is NOT initialized, let's set the time!");
		// When time needs to be set on a new device, or after a power loss, the
		// following line sets the RTC to the date & time this sketch was compiled
		rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
		// This line sets the RTC with an explicit date & time, for example to set
		// January 21, 2014 at 3am you would call:
		// rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
		//
		// Note: allow 2 seconds after inserting battery or applying external power
		// without battery before calling adjust(). This gives the PCF8523's
		// crystal oscillator time to stabilize. If you call adjust() very quickly
		// after the RTC is powered, lostPower() may still return true.
	}
	rtc.start();
	float drift = 43; // seconds plus or minus over oservation period - set to 0 to cancel previous calibration.
	float period_sec = (7 * 86400);  // total obsevation period in seconds (86400 = seconds in 1 day:  7 days = (7 * 86400) seconds )
	float deviation_ppm = (drift / period_sec * 1000000); //  deviation in parts per million (μs)
	float drift_unit = 4.34; // use with offset mode PCF8523_TwoHours
	// float drift_unit = 4.069; //For corrections every min the drift_unit is 4.069 ppm (use with offset mode PCF8523_OneMinute)
	int offset = round(deviation_ppm / drift_unit);
	// rtc.calibrate(PCF8523_TwoHours, offset); // Un-comment to perform calibration once drift (seconds) and observation period (seconds) are correct
	// rtc.calibrate(PCF8523_TwoHours, 0); // Un-comment to cancel previous calibration
	Serial.print("Offset is "); Serial.println(offset); // Print to control offset
	DateTime now = rtc.now();
}

#endif //MAINBOARDWIRED_RTC_H
