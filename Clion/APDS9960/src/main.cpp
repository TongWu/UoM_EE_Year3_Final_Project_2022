/*
 * APDS9960 is different from VCNL4040, it includes an additional gesture sensor, to allow us to detect gesture from
 * objects.
 * Also, it includes color sensor, to detect the RGB value.
 */

#include "Arduino.h"
#include "Adafruit_APDS9960.h"

Adafruit_APDS9960 ProximitySensor;

// Declare global parameters
uint16_t r, g, b, c;
uint16_t Prev_LuxValue = 0;
uint16_t Normal_LuxValue = 0;

void getRGB() {
	// According to the function of APDS9960, read the RGBC value first
	while(!ProximitySensor.colorDataReady()) {
		delay(1);
	}
	ProximitySensor.getColorData(&r, &g, &b, &c);
}

uint16_t CalculateLux() {
	getRGB();
	uint16_t LuxValue;
	LuxValue = ProximitySensor.calculateLux(r, g, b);
	return LuxValue;
}

void NormalLuxValueDetect() {
	uint16_t LuxValue = 0;
	long TotalValue = 0;
	int i = 0;
	for (i = 0; i < 6;) {
		LuxValue = CalculateLux();
		if (Prev_LuxValue == 0)
			Prev_LuxValue = LuxValue;
		if(abs(LuxValue - Prev_LuxValue) <= 35) {
			TotalValue += LuxValue;
			++i;
		} else {
			Serial.println("Read environment lux value failed, retry in 3 seconds...");
			i = 0;
			TotalValue = 0;
			Prev_LuxValue = 0;
			delay(3000);
		}
	}
	Normal_LuxValue = uint16_t (TotalValue / i);
	Serial.print("Normal Lux has been set to ");
	Serial.println(Normal_LuxValue);
}

bool LuxSafeCheck(uint16_t CurrentLuxValue) {
	bool isSafe;
	if(float(abs(CurrentLuxValue-Normal_LuxValue)/Normal_LuxValue) >= 0.8)
		isSafe = true;
	else isSafe = false;
	return isSafe;
}

bool ProximitySafeCheck(uint16_t CurrentProximityValue) {
	bool isSafe;
	if(CurrentProximityValue <= 2)
		isSafe = true;
	else isSafe = false;
	return isSafe;
}

void AdjustNormalLux(uint16_t Current_LuxValue) {
	Normal_LuxValue = uint16_t (Normal_LuxValue*0.2 + Current_LuxValue*0.8);
	Serial.print("Normal Lux Value change to ");
	Serial.println(Normal_LuxValue);
}

bool isLuxSafe = false;
bool isProximitySafe = false;
void APDS9960() {
	uint16_t Current_LuxValue = CalculateLux();
	uint16_t Current_ProximityValue = ProximitySensor.readProximity();
	isLuxSafe = LuxSafeCheck(Current_LuxValue);
	isProximitySafe = ProximitySafeCheck(Current_ProximityValue);
	Serial.print("Lux safe?         ");
	Serial.println(isLuxSafe);
	Serial.print("Proximity Safe?    ");
	Serial.println(isProximitySafe);
	Serial.println(Current_LuxValue);
	Serial.println(Current_ProximityValue);
	/*if (!isProximitySafe) {
		if(!isLuxSafe)
			Serial.println("Stranger Approached!");
		else if(Current_LuxValue <= 3)
			Serial.println("Stranger Approached!");
	} else
		AdjustNormalLux(Current_LuxValue);*/
	AdjustNormalLux(Current_LuxValue);
}

// the setup function runs once when you press reset or power the board
void setup() {
	Serial.begin(9600);

	if(!ProximitySensor.begin()){
		Serial.println("failed to initialize device! Please check your wiring.");
	}
	else Serial.println("Device initialized!");

	// Enable functions in APDS9960
	ProximitySensor.enableProximity(true);
	ProximitySensor.enableColor(true);
	ProximitySensor.enableGesture(true);

	// Start Normal Lux value detection
	NormalLuxValueDetect();
}

// the loop function runs over and over again forever
void loop() {
	// Serial.println(ProximitySensor.readProximity());
	APDS9960();
	delay(500);
}