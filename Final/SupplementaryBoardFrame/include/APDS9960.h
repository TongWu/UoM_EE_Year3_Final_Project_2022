//
// Created by TedWu on 2022/3/13.
//

#ifndef MAINBOARD_APDS9960_H
#define MAINBOARD_APDS9960_H

#include "Arduino.h"
#include "Adafruit_APDS9960.h"


// Create object for APDS9960
Adafruit_APDS9960 apds;
// Initialize RGB value, c for clear color
uint16_t r, g, b, c;

void APDS_Setup() {
	if(!apds.begin()){
		Serial.println("failed to initialize device! Please check your wiring.");
	}
	else Serial.println("Device initialized!");
	//enable proximity mode
	apds.enableProximity(true);
}

/*
 * Read the RGB values
 * write: (uint16_t) r, g, b, c
 */
void getRGB() {
	while (!apds.colorDataReady()) {
		delay(10);
	}
	apds.getColorData(&r, &g, &b, &c);
}

/*
 * Read RGB once, calculate illuminance
 * return: (uint16_t) illuminance (Lux)
 */
uint16_t getLux() {
	getRGB();
	return apds.calculateLux(r, g, b);
}

/*
 * Read RGB few times upon input, calculate average illuminance
 * input: (int) count
 * return: (float) illuminance (Lux)
 */
float CalculateLuxAverage(int count, float offset) {
	uint16_t LuxTotal = 0;
	float LuxAverage = 0;
	uint16_t CurrentLuxValue = 0;
	uint16_t Prev_LuxValue = 0;
	for (int i = 0; i < count-1; ++i) {
		getRGB();
		CurrentLuxValue = apds.calculateLux(r, g, b);
		if (i == 0) {
			LuxTotal += CurrentLuxValue;
			Prev_LuxValue = CurrentLuxValue;
		} else if (abs(CurrentLuxValue - Prev_LuxValue) >= offset) {
			Serial.println("Read environment lux value failed, retry in 3 seconds...");
			i = 0;
			LuxTotal = 0;
			Prev_LuxValue = 0;
			delay(3000);
		} else {
			LuxTotal += CurrentLuxValue;
			Prev_LuxValue = CurrentLuxValue;
		}
	}
	LuxAverage = (float) LuxTotal / (float) count;
	return LuxAverage;
}
/*
 * return: (uint16_t) color temperature (Kelvin)
 */
uint16_t getColorTemperature() {
	getRGB();
	return apds.calculateColorTemperature(r, g, b);
}

/*
 * Read RGB few times upon input, calculate average color temperature
 * input: (int) count
 * return: (float) color temperature (Kelvin)
 */
float CalculateColorTemperatureAverage(int count, float offset) {
	uint16_t ColorTemperatureTotal = 0;
	float ColorTemperatureAverage = 0;
	uint16_t Prev_ColorTemperature = 0;
	uint16_t CurrentColorTemperature = 0;
	for (int i = 0; i < count-1; ++i) {
		getRGB();
		CurrentColorTemperature = apds.calculateColorTemperature(r, g, b);
		if (i == 0) {
			ColorTemperatureTotal += CurrentColorTemperature;
			Prev_ColorTemperature = CurrentColorTemperature;
		} else if (abs(CurrentColorTemperature - Prev_ColorTemperature) >= offset) {
			Serial.println("Average color temperature failed, retry in 3 seconds...");
			i = 0;
			ColorTemperatureTotal = 0;
			Prev_ColorTemperature = 0;
			delay(3000);
		} else {
			ColorTemperatureTotal += CurrentColorTemperature;
			Prev_ColorTemperature = CurrentColorTemperature;
		}
	}
	ColorTemperatureAverage = (float) ColorTemperatureTotal / (float) count;
	return ColorTemperatureAverage;
}

/*
 * Get average RGB by few rounds, determine the environment lux value
 */
float EnvLux = 0;
void DetectEnvLux() {
	EnvLux = CalculateLuxAverage(10, 35);
	Serial.print("Environment Lux value has been set to ");
	Serial.println(EnvLux);
}

/*
 * Check Current and Previous lux value difference
 * return: (bool) isLuxSafe
 */
bool SafeCheckLux(uint16_t Current, uint16_t Prev) {
	bool isLuxSafe = false;
	if( (float) abs(Current - EnvLux) / EnvLux >= 0.8)
		isLuxSafe = true;
	else
		isLuxSafe = false;
	return isLuxSafe;
}

/*
 * Check the Proximity
 * return: (bool) isProximitySafe
 */
bool SafeCheckProximity(uint8_t CurrentProximityValue) {
	bool isProximitySafe;
	if(CurrentProximityValue >= 5)
		isProximitySafe = false;
	else isProximitySafe = true;
	return isProximitySafe;
}

/*
 * Adjust EnvLux Value during each loop
 */
void AdjustEnvLux(uint16_t Current) {
	EnvLux = uint16_t (EnvLux * 0.2 + Current * 0.8);
	Serial.print("Environment Lux Value has been changed to ");
	Serial.println(EnvLux);
}

#endif //MAINBOARD_APDS9960_H
