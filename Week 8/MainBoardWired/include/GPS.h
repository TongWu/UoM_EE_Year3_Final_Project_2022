//
// Created by TedWu on 2022/4/29.
//

#ifndef MAINBOARDWIRED_GPS_H
#define MAINBOARDWIRED_GPS_H

#include <TinyGPSPlus.h>
#include <SoftwareSerial.h>
static const int RXPin = 8, TXPin = 7;
static const uint32_t GPSBaud = 9600;
// The TinyGPSPlus object
TinyGPSPlus gps;

// The serial connection to the GPS device
SoftwareSerial ss(RXPin, TXPin);

void GPSsetup() {
	ss.begin(GPSBaud);
}

void printGPS() {
	Serial.print("Location:");
	if (gps.location.isValid())
	{
		Serial.print(gps.location.lat(), 6);
		Serial.print(F(","));
		Serial.print(gps.location.lng(), 6);
	}
	else
	{
		Serial.print(F("INVALID"));
	}

	Serial.print(F("  Date/Time: "));
	if (gps.date.isValid())
	{
		Serial.print(gps.date.month());
		Serial.print(F("/"));
		Serial.print(gps.date.day());
		Serial.print(F("/"));
		Serial.print(gps.date.year());
	}
	else
	{
		Serial.print(F("INVALID"));
	}

	Serial.print(F(" "));
	if (gps.time.isValid())
	{
		if (gps.time.hour() < 10) Serial.print(F("0"));
		Serial.print(gps.time.hour());
		Serial.print(F(":"));
		if (gps.time.minute() < 10) Serial.print(F("0"));
		Serial.print(gps.time.minute());
		Serial.print(F(":"));
		if (gps.time.second() < 10) Serial.print(F("0"));
		Serial.print(gps.time.second());
		Serial.print(F("."));
		if (gps.time.centisecond() < 10) Serial.print(F("0"));
		Serial.print(gps.time.centisecond());
	}
	else
	{
		Serial.print(F("INVALID"));
	}

}

void getGPS() {
	if (ss.available() > 0){
		if (gps.encode(ss.read())) {
			Serial.println("GPS got");
			printGPS();
		}
		else
			Serial.println("Failed to get GPS");
	}
}

#endif //MAINBOARDWIRED_GPS_H
