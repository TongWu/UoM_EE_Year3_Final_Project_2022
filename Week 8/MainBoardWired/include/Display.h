//
// Created by TedWu on 2022/3/24.
//

#ifndef MAINBOARD_DISPLAY_H
#define MAINBOARD_DISPLAY_H
#include "Arduino.h"
#include "SPI.h"
#include "Wire.h"
#include "Adafruit_GFX.h"
#include "Adafruit_SH110X.h"
Adafruit_SH1107 display = Adafruit_SH1107(64, 128, &Wire);

void displaySetup() {
	Serial.println("128x64 OLED FeatherWing test");
	delay(250); // wait for the OLED to power up
	display.begin(0x3C, true);
	Serial.println("OLED begun");
	display.display();
	delay(1000);
	display.clearDisplay();
	display.display();
	display.setRotation(1);
	Serial.println("Button test");
	display.setTextSize(1);
	display.setTextColor(SH110X_WHITE);
	//display.setCursor(0,0);
	//display.print("ToF\nRoll\nPitch");
	//display.display(); // actually display all of the above
}


#endif //MAINBOARD_DISPLAY_H
