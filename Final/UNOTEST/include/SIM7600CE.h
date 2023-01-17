//
// Created by TedWu on 2022/3/13.
//

#ifndef MAINBOARD_SIM7600CE_H
#define MAINBOARD_SIM7600CE_H

#include "Arduino.h"
#include "SoftwareSerial.h"
#include "RTC.h"
#include "DFRobot_SIM7000.h"
SoftwareSerial SIM7600CE(0, 1);
//DFRobot_SIM7000 SIM(&SIM7600CE);
boolean newData = false;
const byte numChars = 32;
char receivedChars[numChars];   // an array to store the received data

void readContent() {
	int i=0;
	SIM7600CE.println("AT+CGPSINFO");
	while (1) {
		if (SIM7600CE.available()) {
			char c = SIM7600CE.read();
			receivedChars[i] = c;
			i++;
			if (i==numChars) break;
		}
	}
}

void SendMessage(int board, int event) {
	SIM7600CE.println("AT+CGPS=1");
	delay(100000);
	readContent();
	SIM7600CE.println("AT+CGPS=0");
	delay(1000);
	SIM7600CE.println("AT+CMGF=1");
	delay(1000);
	SIM7600CE.println("AT+CMGS=\"00447738425764\"\r");
	delay(1000);
	SIM7600CE.print("Position:");
	delay(100);
	SIM7600CE.print(receivedChars);
	delay(100);
	SIM7600CE.println((char)26);
	delay(1000);
}

void SIM7600CE_Setup() {
	SIM7600CE.begin(115200);
	SIM7600CE.println("AT+CPOWD=1");
}



#endif //MAINBOARD_SIM7600CE_H
