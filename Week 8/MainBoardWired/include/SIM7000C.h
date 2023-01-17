//
// Created by TedWu on 2022/5/4.
//

#ifndef MAINBOARDWIRED_SIM7000C_H
#define MAINBOARDWIRED_SIM7000C_H
#include <DFRobot_SIM7000.h>
#include "RTC.h"
#define PIN_TX     7
#define PIN_RX     8
SoftwareSerial     mySerial(PIN_RX,PIN_TX);
DFRobot_SIM7000    sim7000(&mySerial);

void SIM7000C_setup() {
	mySerial.begin(19200);
	Serial.println("Turn ON SIM7000......");
	if(sim7000.turnON()){                                          //Turn ON SIM7000
		Serial.println("Turn ON !");
	}
	Serial.println("Set baud rate......");
	while(1){
		if(sim7000.setBaudRate(19200)){                            //Set SIM7000 baud rate from 115200 to 19200 reduce the baud rate to avoid distortion
			Serial.println("Set baud rate:19200");
			break;
		}else{
			Serial.println("Faile to set baud rate");
			delay(1000);
		}
	}
	Serial.println("Check SIM card......");
	if(sim7000.checkSIMStatus()){                                  //Check SIM card
		Serial.println("SIM card READY");
	}else{
		Serial.println("SIM card ERROR, Check if you have insert SIM card and restart SIM7000");
		while(1);
	}
	Serial.println("Init positioning function......");
	while(1){
		if(sim7000.initPos()){
			Serial.println("Positioning function initialized");
			break;
		}else{
			Serial.println("Fail to init positioning function");
			delay(1000);
		}
	}
}

void getPosition() {
	Serial.println("Getting position......");
	if(sim7000.getPosition()){                                     //Get the current position
		Serial.print(" Longitude : ");
		Serial.println(sim7000.getLongitude());                    //Get longitude
		Serial.print(" Latitude : ");
		Serial.println(sim7000.getLatitude());                     //Get latitude
	}else{
		Serial.println("Wrong data try again");
	}
}

void SendMessage(int board, int event) {
	char boardInfo;
	char eventInfo;
	DateTime now = rtc.now();
	mySerial.println("AT+CMGF=1");
	delay(1000);
	mySerial.println("AT+CMGS=\"00447738425764\"\r");
	delay(1000);
	mySerial.print("Theft Occur: ");
	delay(100);
	//mySerial.print(board);
	switch (board) {
		case 0:
			mySerial.print("Main Board");
			break;
		case 1:
			mySerial.print("Supplementary Board 1");
			break;
		case 2:
			mySerial.print("Supplementary Board 2");
			break;
	}
	delay(100);
	mySerial.print(", ");
	delay(100);
	//mySerial.print(event);
	switch (event) {
		case 1:
			mySerial.print("Hall Sensor");
			break;
		case 2:
			mySerial.print("Throttle");
			break;
		case 3:
			mySerial.print("Hall Sensor & Throttle");
			break;
		case 4:
			mySerial.print("Proximity & Roll & Pitch");
			break;
		case 5:
			mySerial.print("Proximity & Roll");
			break;
		case 6:
			mySerial.print("Proximity & Pitch");
			break;
		case 7:
			mySerial.print("Roll & Pitch");
			break;
		case 8:
			mySerial.print("Proximity");
			break;
		case 9:
			mySerial.print("Roll");
			break;
		case 10:
			mySerial.print("Pitch");
			break;
	}
	mySerial.println();
	delay(100);
	mySerial.print(". ");
	delay(100);
	mySerial.print("At date and time: ");
	delay(100);
	mySerial.print("2022/");
	delay(100);
	mySerial.print(now.month(), DEC);
	delay(100);
	mySerial.print("/");
	delay(100);
	mySerial.print(now.day(), DEC);
	delay(100);
	mySerial.print(", ");
	delay(100);
	mySerial.print(daysOfTheWeek[now.dayOfTheWeek()]);
	delay(100);
	mySerial.print(", ");
	delay(100);
	mySerial.print(now.hour(), DEC);
	delay(100);
	mySerial.print(":");
	delay(100);
	mySerial.print(now.minute(), DEC);
	delay(100);
	mySerial.print(":");
	delay(100);
	mySerial.println(now.second(), DEC);
	delay(100);
	mySerial.println((char)26);
	delay(1000);
}


#endif //MAINBOARDWIRED_SIM7000C_H
