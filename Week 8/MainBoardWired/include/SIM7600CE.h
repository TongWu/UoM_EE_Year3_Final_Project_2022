//
// Created by TedWu on 2022/3/13.
//

#ifndef MAINBOARD_SIM7600CE_H
#define MAINBOARD_SIM7600CE_H

#include "Arduino.h"
#include "SoftwareSerial.h"
#include "RTC.h"
SoftwareSerial SIM7600CE(0, 1);

void SendMessage(int board, int event) {
	char boardInfo;
	char eventInfo;
	DateTime now = rtc.now();
	SIM7600CE.println("AT+CMGF=1");
	delay(500);
	SIM7600CE.println("AT+CMGS=\"00447738425764\"\r");
	delay(500);
	SIM7600CE.print("Theft Occur: ");
	delay(100);
	//SIM7600CE.print(board);
	switch (board) {
		case 0:
			SIM7600CE.print("Main Board");
			break;
		case 1:
			SIM7600CE.print("Supplementary Board 1");
			break;
		case 2:
			SIM7600CE.print("Supplementary Board 2");
			break;
	}
	delay(50);
	SIM7600CE.print(", ");
	delay(50);
	//SIM7600CE.print(event);
	switch (event) {
		case 1:
			SIM7600CE.print("Hall Sensor");
			break;
		case 2:
			SIM7600CE.print("Throttle");
			break;
		case 3:
			SIM7600CE.print("Hall Sensor & Throttle");
			break;
		case 4:
			SIM7600CE.print("Proximity & Roll & Pitch");
			break;
		case 5:
			SIM7600CE.print("Proximity & Roll");
			break;
		case 6:
			SIM7600CE.print("Proximity & Pitch");
			break;
		case 7:
			SIM7600CE.print("Roll & Pitch");
			break;
		case 8:
			SIM7600CE.print("Proximity");
			break;
		case 9:
			SIM7600CE.print("Roll");
			break;
		case 10:
			SIM7600CE.print("Pitch");
			break;
	}
	SIM7600CE.println();
	delay(50);
	SIM7600CE.print(". ");
	delay(50);
	SIM7600CE.print("At date and time: ");
	delay(50);
	SIM7600CE.print("2022/");
	delay(50);
	SIM7600CE.print(now.month(), DEC);
	delay(50);
	SIM7600CE.print("/");
	delay(50);
	SIM7600CE.print(now.day(), DEC);
	delay(50);
	SIM7600CE.print(", ");
	delay(50);
	SIM7600CE.print(daysOfTheWeek[now.dayOfTheWeek()]);
	delay(50);
	SIM7600CE.print(", ");
	delay(50);
	SIM7600CE.print(now.hour(), DEC);
	delay(50);
	SIM7600CE.print(":");
	delay(50);
	SIM7600CE.print(now.minute(), DEC);
	delay(50);
	SIM7600CE.print(":");
	delay(50);
	SIM7600CE.println(now.second(), DEC);
	delay(50);
	SIM7600CE.println((char)26);
	delay(500);
}

void SIM7600CE_Setup() {
	SIM7600CE.begin(115200);
}



#endif //MAINBOARD_SIM7600CE_H
