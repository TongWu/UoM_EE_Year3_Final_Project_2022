/*
 * Important Information:
 * The code and the pin-outs may not suitable for Arduino UNO Wi-Fi Rev.2
 * All the code, header files and pin-outs will be optimised once the UNO Wi-Fi Rev.2 has been delivered
 */

/*
 * Testing Excel has been saved
 * Result:
 *
 * When distance is less than 3 cm, equivalent real value is bigger than 32, use algorithm below:
 * y = 510.546 * exp(-0.948399 * x)
 * When distance is less than 10 cm and large than 3 cm, equivalent real value is bigger than 3 and less than 20, use algorithm below:
 * y = 88.5581 * exp(-0.383196 * x)
 * When distance is less than 15 cm and large than 10 cm, equivalent real value is bigger than 1 and less than 3, use algorithm below:
 * y = 32.768 * exp(-0.693147 * x)
 */
#include <Arduino.h>
#include "Adafruit_VCNL4040.h"

/*
 * VCNL4040 has maximum detecting distance as 200 mm, equivalent with 20 cm
 */

// Create the Proximity Sensor class object
Adafruit_VCNL4040 ProximitySensor = Adafruit_VCNL4040();
// Initialize global parameters
uint16_t Prev_LuxValue = 0;
uint16_t Normal_LuxValue = 0;
bool Prev_isSafe = false;

void NormalLuxValueDetect() {
	uint16_t LuxValue = 0;
	long TotalValue = 0;
	int i=0;
	for(i=0; i<6;) {
		LuxValue = ProximitySensor.getLux();
		if (Prev_LuxValue == 0)
			Prev_LuxValue = LuxValue;
		/*Serial.print("Lux value is ");
		Serial.println(LuxValue);
		Serial.print("Prev_Lux value is ");
		Serial.println(Prev_LuxValue);*/
		if(abs(LuxValue-Prev_LuxValue) <= 600) {
			TotalValue += LuxValue;
			++i;
			// Serial.println(TotalValue);
		}
		else {
			Serial.println("Lux sensor read environment lux value failed, retry in 3 seconds...");
			i = 0;
			TotalValue = 0;
			delay(3000);
		}
	}
	Normal_LuxValue = uint16_t (TotalValue / i);
	Serial.print("Normal Lux Value has been set to ");
	Serial.println(Normal_LuxValue);
}

bool LuxSafeStateDetect(uint16_t InputValue) {
	/*
	 * This is the function to achieve the detection of lux sensor to determine whether it is under the safety status,
	 * by using the derivative status to introduce the method
	 */
	/*
	 * 此函数当前阶段只实现在波动不大的正常光照中，如果出现异常光照，则判定为不安全状态
	 */
	// Compare the current status and the Prev Status
	uint16_t CurrentLuxValue = InputValue;
	bool isSafe = false;
	if(abs(CurrentLuxValue - Normal_LuxValue) >= 300) {
		//detect the previous isSafe state, if true, detect
		if(!Prev_isSafe) {
			isSafe = false;
		} else{
			if(abs(CurrentLuxValue-Prev_LuxValue) >= 600) {
				isSafe = false;
			} else {
				if(ProximitySensor.getProximity()<6) {
					Normal_LuxValue = (CurrentLuxValue + Prev_LuxValue + Normal_LuxValue) / 3;
					Serial.print("Environment Lux value changed, now the Normal lux value has been set to ");
					Serial.println(Normal_LuxValue);
					isSafe = true;
				} else Serial.println("Need Accelerometer, Gyroscope, even camera or bluetooth scanning");
			}
		}
	}
	// Store the current status value to previous state
	Prev_LuxValue = CurrentLuxValue;
	Prev_isSafe = isSafe;
	// Serial.println(isSafe);
	return isSafe;
}

bool LuxSafeAlgorithm(uint16_t CurrentLuxValue) {
	/*
	 * 此算法在后期应用决策树算法实现
	 * This Algorithm should be achieved by Decision-tree
	 */
	/*bool isSafe = false;
	if(abs(CurrentLuxValue - Prev_LuxValue) >= 300) {
		if(abs(CurrentLuxValue - Normal_LuxValue) >= 400) {
			isSafe = false;
		} else {
			if(Prev_isSafe) {
				isSafe = true;
				Serial.println("Adjustment should be done");
			} else {
				isSafe = false;
			}
		}
	} else {
		if(abs(CurrentLuxValue - Normal_LuxValue) >= 400) {
			if (Prev_isSafe) {
				isSafe = true;
				Serial.println("Adjustment should be done");
			} else {isSafe = false;}
		} else {isSafe = true;}
	}
	Prev_LuxValue = CurrentLuxValue;
	Prev_isSafe = isSafe;*/
	bool isSafe = false;
	if (abs(CurrentLuxValue-Normal_LuxValue)<=300)
		isSafe = true;
	else isSafe = false;
	return isSafe;
}

void AdjustNormalLux(uint16_t CurrentValue) {
	Normal_LuxValue = Normal_LuxValue*0.2 + CurrentValue*0.8;
	Serial.println("Normal Lux Value change to ");
	Serial.println(Normal_LuxValue);
}

void VCNL4040Algorithm() {
	/*
	 * Implement the lux value and the proximity value to determine whether the person has been approached
	 */
	// Initialize parameters
	bool isLuxSafe = false;
	// Read Proximity value
	uint16_t ProximityRealValue = ProximitySensor.getProximity();
	// Read lux value
	uint16_t LuxValue = ProximitySensor.getLux();
	// Algorithm Part
	/*
	 * In order to alarm the controller to do the actions when:
	 * 1. The stranger approaches in 4 cm, at the same time, the lux sensor should be lower than the normal value.
	 * According to the Proximity sensor testing values sheet, the possible situations should as listed below:
	 */
	// Read the lux sensor first, make sure it value is under the safe status
	isLuxSafe = LuxSafeAlgorithm(LuxValue);
	if(ProximityRealValue >= 3) {
		Serial.println(ProximityRealValue);
		if(!isLuxSafe) {
			Serial.println("Stranger Approached");
		}
	} else
		AdjustNormalLux(LuxValue);
}

void setup() {
	// write your initialization code here
	Serial.begin(9600);
	while (!Serial)
		delay(1);

	if (!ProximitySensor.begin()) {
		Serial.println("Couldn't find VCNL4040 chip");
		while (1);
	}
	Serial.println("Found VCNL4040 chip");

	//vcnl4040.setProximityLEDCurrent(VCNL4040_LED_CURRENT_200MA);
	Serial.print("Proximity LED current set to: ");
	switch(ProximitySensor.getProximityLEDCurrent()) {
		case VCNL4040_LED_CURRENT_50MA: Serial.println("50 mA"); break;
		case VCNL4040_LED_CURRENT_75MA: Serial.println("75 mA"); break;
		case VCNL4040_LED_CURRENT_100MA: Serial.println("100 mA"); break;
		case VCNL4040_LED_CURRENT_120MA: Serial.println("120 mA"); break;
		case VCNL4040_LED_CURRENT_140MA: Serial.println("140 mA"); break;
		case VCNL4040_LED_CURRENT_160MA: Serial.println("160 mA"); break;
		case VCNL4040_LED_CURRENT_180MA: Serial.println("180 mA"); break;
		case VCNL4040_LED_CURRENT_200MA: Serial.println("200 mA"); break;
	}

	//vcnl4040.setProximityLEDDutyCycle(VCNL4040_LED_DUTY_1_40);
	Serial.print("Proximity LED duty cycle set to: ");
	switch(ProximitySensor.getProximityLEDDutyCycle()) {
		case VCNL4040_LED_DUTY_1_40: Serial.println("1/40"); break;
		case VCNL4040_LED_DUTY_1_80: Serial.println("1/80"); break;
		case VCNL4040_LED_DUTY_1_160: Serial.println("1/160"); break;
		case VCNL4040_LED_DUTY_1_320: Serial.println("1/320"); break;
	}

	//vcnl4040.setAmbientIntegrationTime(VCNL4040_AMBIENT_INTEGRATION_TIME_80MS);
	Serial.print("Ambient light integration time set to: ");
	switch(ProximitySensor.getAmbientIntegrationTime()) {
		case VCNL4040_AMBIENT_INTEGRATION_TIME_80MS: Serial.println("80 ms"); break;
		case VCNL4040_AMBIENT_INTEGRATION_TIME_160MS: Serial.println("160 ms"); break;
		case VCNL4040_AMBIENT_INTEGRATION_TIME_320MS: Serial.println("320 ms"); break;
		case VCNL4040_AMBIENT_INTEGRATION_TIME_640MS: Serial.println("640 ms"); break;
	}


	//vcnl4040.setProximityIntegrationTime(VCNL4040_PROXIMITY_INTEGRATION_TIME_8T);
	Serial.print("Proximity integration time set to: ");
	switch(ProximitySensor.getProximityIntegrationTime()) {
		case VCNL4040_PROXIMITY_INTEGRATION_TIME_1T: Serial.println("1T"); break;
		case VCNL4040_PROXIMITY_INTEGRATION_TIME_1_5T: Serial.println("1.5T"); break;
		case VCNL4040_PROXIMITY_INTEGRATION_TIME_2T: Serial.println("2T"); break;
		case VCNL4040_PROXIMITY_INTEGRATION_TIME_2_5T: Serial.println("2.5T"); break;
		case VCNL4040_PROXIMITY_INTEGRATION_TIME_3T: Serial.println("3T"); break;
		case VCNL4040_PROXIMITY_INTEGRATION_TIME_3_5T: Serial.println("3.5T"); break;
		case VCNL4040_PROXIMITY_INTEGRATION_TIME_4T: Serial.println("4T"); break;
		case VCNL4040_PROXIMITY_INTEGRATION_TIME_8T: Serial.println("8T"); break;
	}

	//vcnl4040.setProximityHighResolution(false);
	Serial.print("Proximity measurement high resolution? ");
	Serial.println(ProximitySensor.getProximityHighResolution() ? "True" : "False");

	Serial.println("");

	// Start detect the normal lux value
	// Prev_LuxValue = ProximitySensor.getLux();
	NormalLuxValueDetect();
}

void loop() {
	// Run the Proximity Sensor Algorithm
	VCNL4040Algorithm();
	delay(500);
}
