//
// Created by TedWu on 2022/3/13.
//

#ifndef MAINBOARD_SIM7600CE_H
#define MAINBOARD_SIM7600CE_H

#include "Arduino.h"
#include "SoftwareSerial.h"
#include "RTC.h"
SoftwareSerial SIM7600CE(0, 1);

void SendMessage(char board, char event) {
	DateTime now = rtc.now();
	SIM7600CE.println("AT+CMGF=1");
	delay(1000);
	SIM7600CE.println("AT+CMGS=\"00447738425764\"\r");
	delay(1000);
	SIM7600CE.print("Theft Occur: ");
	delay(100);
	SIM7600CE.print(board);
	delay(100);
	SIM7600CE.print(', ');
	delay(100);
	SIM7600CE.print(event);
	delay(100);
	SIM7600CE.print('. ');
	delay(100);
	SIM7600CE.print('At time: ');
	delay(100);
	SIM7600CE.print(now.month(), DEC);
	delay(100);
	SIM7600CE.print(now.day(), DEC);
	delay(100);
	SIM7600CE.print(daysOfTheWeek[now.dayOfTheWeek()]);
	delay(100);
	SIM7600CE.print(now.hour(), DEC);
	delay(100);
	SIM7600CE.print(now.minute(), DEC);
	delay(100);
	SIM7600CE.println(now.second(), DEC);
	delay(100);
	SIM7600CE.println((char)26);
	delay(1000);
}

void SIM7600CE_Setup() {
	SIM7600CE.begin(115200);
}


#endif //MAINBOARD_SIM7600CE_H
