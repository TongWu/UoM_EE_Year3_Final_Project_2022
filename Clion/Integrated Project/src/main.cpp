#include <Arduino.h>
#include "Adafruit_LSM6DS33.h"
#include "DFRobot_ID809_I2C.h"
#include "Adafruit_VCNL4040.h"
#include "Adafruit_APDS9960.h"

/* FingerPrint Part */
#define COLLECT_NUMBER 3  //Fingerprint sampling times, can be set to 1-3
#define IRQ         6  //IRQ pin
DFRobot_ID809_I2C fingerprint;
void FingerprintReg() {
	uint8_t ID,i;
	/*Compare the captured fingerprint with all fingerprints in the fingerprint library
	  Return fingerprint ID number(1-80) if succeed, return 0 when failed
	  Function: clear the last captured fingerprint image
	 */
	fingerprint.search();       //Can add "if else" statement to judge whether the fingerprint has been registered.
	/*Get a unregistered ID for saving fingerprint
	  Return ID number when succeed
	  Return ERR_ID809 if failed
	 */
	while ((ID = fingerprint.getEmptyID()) == ERR_ID809){
			/*Get error code imformation*/
			//desc = fingerprint.getErrorDescription();
			//Serial.println(desc);
			delay(1000);
	}
	Serial.print("Unregistered ID,ID=");
	Serial.println(ID);
	i = 0;   //Clear sampling times
	/*Fingerprint Sampling 3 times */
	while(i < COLLECT_NUMBER){
		/*Set fingerprint LED ring to breathing lighting in blue*/
		fingerprint.ctrlLED(/*LEDMode = */fingerprint.eBreathing, /*LEDColor = */fingerprint.eLEDBlue, /*blinkCount = */0);
		Serial.print("The fingerprint sampling of the");
		Serial.print(i+1);
		Serial.println("(th) time is being taken");
		Serial.println("Please press down your finger");
		/*Capture fingerprint image, 10s idle timeout
		  If succeed return 0, otherwise return ERR_ID809
		 */
		if((fingerprint.collectionFingerprint(/*timeout = */10)) != ERR_ID809){
			/*Set fingerprint LED ring to quick blink in yellow 3 times*/
			fingerprint.ctrlLED(/*LEDMode = */fingerprint.eFastBlink, /*LEDColor = */fingerprint.eLEDYellow, /*blinkCount = */3);
			Serial.println("Capturing succeeds");
			i++;   //Sampling times +1
		}else{
			Serial.println("Capturing fails");
			/*Get error code information*/
			//desc = fingerprint.getErrorDescription();
			//Serial.println(desc);
		}
		Serial.println("Please release your finger");
		/*Wait for finger to release
		  Return 1 when finger is detected, otherwise return 0
		 */
		while(fingerprint.detectFinger());
	}

	/*Save fingerprint information into an unregistered ID*/
	if(fingerprint.storeFingerprint(/*Empty ID = */ID) != ERR_ID809){
		Serial.print("Saving succeed,ID=");
		Serial.println(ID);
		/*Set fingerprint LED ring to always ON in green*/
		fingerprint.ctrlLED(/*LEDMode = */fingerprint.eKeepsOn, /*LEDColor = */fingerprint.eLEDGreen, /*blinkCount = */0);
		delay(1000);
		/*Turn off fingerprint LED ring */
		fingerprint.ctrlLED(/*LEDMode = */fingerprint.eNormalClose, /*LEDColor = */fingerprint.eLEDBlue, /*blinkCount = */0);
	}else{
		Serial.println("Saving failed");
		/*Get error code information*/
		//desc = fingerprint.getErrorDescription();
		//Serial.println(desc);
	}
	Serial.println("-----------------------------");
};
bool FingerprintMatching() {
	/*Compare the captured fingerprint with all fingerprints in the fingerprint library
    Return fingerprint ID number(1-80) if succeed, return 0 when failed
   */
	bool state = false;
	uint8_t ret = fingerprint.search();
	if(ret != 0){
		/*Set fingerprint LED ring to always ON in green*/
		fingerprint.ctrlLED(/*LEDMode = */fingerprint.eKeepsOn, /*LEDColor = */fingerprint.eLEDGreen, /*blinkCount = */0);
		Serial.print("Successfully matched,ID=");
		Serial.println(ret);
		state = true;
	}else{
		/*Set fingerprint LED Ring to always ON in red*/
		fingerprint.ctrlLED(/*LEDMode = */fingerprint.eKeepsOn, /*LEDColor = */fingerprint.eLEDRed, /*blinkCount = */0);
		Serial.println("Matching failed");\
		state = false;
	}
	delay(1000);
	/*Turn off fingerprint LED Ring*/
	fingerprint.ctrlLED(/*LEDMode = */fingerprint.eNormalClose, /*LEDColor = */fingerprint.eLEDBlue, /*blinkCount = */0);
	Serial.println("-----------------------------");
	return state;
};
void FingerprintDeletion() {
	uint8_t ret;
	/*Compare the captured fingerprint with all the fingerprints in the fingerprint library
	  Return fingerprint ID(1-80) if succeed, return 0 when failed
	 */
	ret = fingerprint.search();
	if(ret){
		/*Set fingerprint LED ring to always ON in green*/
		fingerprint.ctrlLED(/*LEDMode = */fingerprint.eKeepsOn, /*LEDColor = */fingerprint.eLEDGreen, /*blinkCount = */0);
		fingerprint.delFingerprint(ret);
		Serial.print("deleted fingerprint,ID=");
		Serial.println(ret);
	}else{
		/*Set fingerprint LED ring to always ON in red*/
		fingerprint.ctrlLED(/*LEDMode = */fingerprint.eKeepsOn, /*LEDColor = */fingerprint.eLEDRed, /*blinkCount = */0);
		Serial.println("Matching failed or the fingerprint hasn't been registered");
	}
	delay(1000);
	/*Turn off fingerprint LED ring*/
	fingerprint.ctrlLED(/*LEDMode = */fingerprint.eNormalClose, /*LEDColor = */fingerprint.eLEDBlue, /*blinkCount = */0);
	Serial.println("-----------------------------");
};
/* FingerPrint Part End */

/* VCNL4040 Part */
/*
 * The VCNL 4040 chip has I2C address 0x60, which is override the address of the sensor in the Arduino Uno WiFi Rev.2
 * So the VCNL4040 chip will not used in the main board, while it may be used on the supplementary board
 */
// Create the Proximity Sensor class object
Adafruit_VCNL4040 ProximitySensor = Adafruit_VCNL4040();
// Initialize global parameters
uint16_t Prev_LuxValue = 0;
uint16_t Normal_LuxValue = 0;
bool Prev_isSafe = false;
void VCNL_NormalLuxValueDetect() {
	uint16_t LuxValue = 0;
	long TotalValue = 0;
	int i=0;
	for(i=0; i<6;) {
		LuxValue = ProximitySensor.getLux();
		if (Prev_LuxValue == 0)
			Prev_LuxValue = LuxValue;
		// Serial.print("Lux value is ");
		// Serial.println(LuxValue);
		// Serial.print("Prev_Lux value is ");
		// Serial.println(Prev_LuxValue);
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
bool VCNL_LuxSafeStateDetect(uint16_t InputValue) {
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
bool VCNL_LuxSafeAlgorithm(uint16_t CurrentLuxValue) {
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
void VCNL_AdjustNormalLux(uint16_t CurrentValue) {
	Normal_LuxValue = Normal_LuxValue*0.2 + CurrentValue*0.8;
	// Serial.println("Normal Lux Value change to ");
	// Serial.println(Normal_LuxValue);
}
bool VCNL4040() {
	/*
	 * Implement the lux value and the proximity value to determine whether the person has been approached
	 */
	// Initialize parameters
	bool isLuxSafe = false;
	bool isProximitySafe = false;
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
	isLuxSafe = VCNL_LuxSafeAlgorithm(LuxValue);
	if(ProximityRealValue >= 3) {
		// Serial.println(ProximityRealValue);
		if(!isLuxSafe) {
			/*Serial.println("Stranger Approached");*/
			isProximitySafe = false;
		}
	} else {
		VCNL_AdjustNormalLux(LuxValue);
		isProximitySafe = true;
	}
	return isProximitySafe;
}
/* VCNL4040 Part End */

/* APDS9960 Part */
Adafruit_APDS9960 apds;
// Declare global parameters
uint16_t r, g, b, c;
/*uint16_t Prev_LuxValue = 0;
uint16_t Normal_LuxValue = 0;*/
void getRGB() {
	// According to the function of APDS9960, read the RGBC value first
	while(!apds.colorDataReady()) {
		delay(1);
	}
	apds.getColorData(&r, &g, &b, &c);
}
uint16_t CalculateLux() {
	getRGB();
	uint16_t LuxValue;
	LuxValue = apds.calculateLux(r, g, b);
	return LuxValue;
}
void EnvLuxValueDetect() {
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
	Serial.print("Environment Lux Value has been set to ");
	Serial.println(Normal_LuxValue);
}
bool LuxSafeCheck(uint16_t CurrentLuxValue) {
	bool isSafe;
	if(float(abs(CurrentLuxValue-Normal_LuxValue)/Normal_LuxValue) >= 0.8)
		isSafe = true;
	else isSafe = false;
	return isSafe;
}
bool ProximitySafeCheck(uint8_t CurrentProximityValue) {
	bool isSafe;
	if(CurrentProximityValue >= 5)
		isSafe = false;
	else isSafe = true;
	return isSafe;
}
void AdjustEnvLux(uint16_t Current_LuxValue) {
	Normal_LuxValue = uint16_t (Normal_LuxValue*0.2 + Current_LuxValue*0.8);
	Serial.print("Normal Lux Value change to ");
	Serial.println(Normal_LuxValue);
}

/* Accelerometer & Gyroscope Part */
// For SPI mode, we need a CS pin
#define LSM_CS 10
// For software-SPI mode we need SCK/MOSI/MISO pins
#define LSM_SCK 13
#define LSM_MISO 12
#define LSM_MOSI 11
Adafruit_LSM6DS33 AccGyro;
float accel_CurrentStateX, accel_CurrentStateY, accel_CurrentStateZ;
float accel_PrevStateX = 0;
float accel_PrevStateY = 0;
float accel_PrevStateZ = 0;
float gyro_MotionValueX = 0;
float gyro_MotionValueY = 0;
float gyro_MotionValueZ = 0;
sensors_event_t accel, gyro, temp;
float accel_CompareDifference() {
	float accel_Difference = abs(accel_CurrentStateX - accel_PrevStateX) + abs(accel_CurrentStateY - accel_PrevStateY) +
	                         abs(accel_CurrentStateZ - accel_PrevStateZ);
	accel_PrevStateX = accel_CurrentStateX;
	accel_PrevStateY = accel_CurrentStateY;
	accel_PrevStateZ = accel_CurrentStateZ;
	//Serial.println(accel_Difference);
	return accel_Difference;
}
double gyro_MotionState() {
	bool isMotion = false;
	float gyro_CurrentValueX = gyro.gyro.x;
	float gyro_CurrentValueY = gyro.gyro.y;
	float gyro_CurrentValueZ = gyro.gyro.z;
	/*if (abs(gyro_CurrentValueX) + abs(gyro_CurrentValueY) + abs(gyro_CurrentValueZ) >= 2) {
		gyro_MotionValueX = gyro_CurrentValueX;
		gyro_MotionValueY = gyro_CurrentValueY;
		gyro_MotionValueZ = gyro_CurrentValueZ;
		isMotion = true;
	}*/
	return abs(gyro_CurrentValueX) + abs(gyro_CurrentValueY) + abs(gyro_CurrentValueZ);
}
void MotionAlgorithm() {
	char MotionState = 0;
	// In order to detect the left leaning, right leaning, etc. The Machine Learning method should be considered, for
	// example, K-means algorithm and other clustering algorithm should be implemented into the function by using tensorflow or pycharm


	switch(MotionState) {
		case 'L':
			Serial.println("The sensor left leaning");
			break;
		case 'R':
			Serial.println("The sensor right leaning");
			break;
		case 'U':
			Serial.println("The sensor up-hold");
			break;
		case 'F':
			Serial.println("The sensor falling-down");
			break;
		case 'S':
			Serial.println("The sensor shaking");
			break;
	}
}
double LSM6DS33_Algorithm() {
	/*
	 * This is the algorithm to monitor whether the sensor has been moved in horizontal
	 */
	// Create three events, accelerometer, gyroscope and temperature

	/*
	 * When the gyroscope changes, means that the sensor has been moved in a particular speed, then read the accelerometer
	 * if the gyroscope speed is in particular range(human action), detect the accelerometer, PIR and proximity sensor
	 * if the gyroscope output is low but not zero, read proximity sensor to detect whether is the wind or other natural actions
	 */
	// First implement the basic situation which ignored the natual impact
	/*if (abs(gyro.gyro.x) >= 0.1 || abs(gyro.gyro.y) >= 0.1 || abs(gyro.gyro.z) >= 0.1)
		// TODO:
		//  Try to implement three gyroscope output value into one integrated value*/
	// Keep monitor the accelerometer, if the current state of the accelerometer has big different with the prev_state, read gyro
	// Initialize all the parameters
	AccGyro.getEvent(&accel, &gyro, &temp);
	accel_CurrentStateX = accel.acceleration.x;
	accel_CurrentStateY = accel.acceleration.y;
	accel_CurrentStateZ = accel.acceleration.z;
	// Serial.println(accel_CurrentStateX);
	float accel_difference = accel_CompareDifference();
	// if(accel_difference >= 2.5) {
		// Read gyroscope
		// if (gyro_MotionState()) {
			// If the sensor motion has been confirmed
			// Do the Motion Algorithm to analyze the motion type
			// MotionAlgorithm();
			// Analyzed as the stranger approached
			// Serial.println("Stranger approached!");
		// }
	// }
	return accel_difference;
}
/* Accelerometer & Gyroscope Part End */

/* Hall Sesnor Part */
double HallSafeCount = 0;
bool Prev_state = false;
double ReadHallSensorVoltage () {
	double HallSensorVoltage = analogRead(A0) * (5.0 / 1023.0);
	return HallSensorVoltage;
}
double HallSensorMonitor() {
		/*
		// Monitor the Hall sensor voltage
		*
		 * Check the voltage change with this sequence:
		 * LOW -> HIGH -> LOW -> HIGH -> LOW -> HIGH -> LOW -> HIGH -> LOW -> HIGH
		 *
		while(ReadHallSensorVoltage() <= 2) {
			while (ReadHallSensorVoltage() >= 4)
				while (ReadHallSensorVoltage() <= 2)
					while (ReadHallSensorVoltage() >= 4)
						while (ReadHallSensorVoltage() <= 2)
							while (ReadHallSensorVoltage() >= 4)
								while (ReadHallSensorVoltage() <= 2)
									while (ReadHallSensorVoltage() >= 4)
										while (ReadHallSensorVoltage() <= 2)
											while (ReadHallSensorVoltage() >= 4) {
												Serial.println("Wheel Moved");
												break;
											}
		}
		*/
		// Determine the motion state
		bool CurrentState;
		if(ReadHallSensorVoltage() <= 2)
			CurrentState = false; // False for down state for Hall Sensor
		else
			CurrentState = true;
		if(CurrentState&&Prev_state)
			HallSafeCount -= 0.1;
		else
			HallSafeCount ++;
		return HallSafeCount;
}
/* Hall Sensor Part End */

/* Throttle Part */
#define THROTTLE_PIN 11
void ThrottleMonitor () {
	int throttle_voltage = analogRead(A0) ;
	Serial.println(throttle_voltage );
	analogWrite(THROTTLE_PIN, throttle_voltage /4 );
}
double ThrottleVoltage() {
	return analogRead(A0) / 4.0;
}
/* Throttle Part End */

/* Buzzer Part */
const int BUZZER_PIN = 3;
void StartBuzzer() {
	digitalWrite(BUZZER_PIN, HIGH);
}
void StopBuzzer() {
	digitalWrite(BUZZER_PIN, LOW);
}


int count = 0;
bool isLocked = false;
double HallSafeState = 0;
double AccDifference = 0;
int flag = 0;
/*
* 0 == safe
* 1 == Hall Sensor
* 2 == Gyro & Acc
* 3 == Proximity
* 4 == Throttle
* 12 == Hall & Gyro
* 13 == Hall & Proximity
* 14 == Hall & Throttle
* 23 == Gyro & Proximity
* 24 == Gyro & Throttle
* 34 == Proximity & Throttle
* 123 == Hall & Gyro & Proximity
* 124 == Hall & Gyro & Throttle
* 134 == Hall & Proximity & Throttle
* 234 == Gyro & Proximity & Throttle
*/
int EventFlag[7] = {0};
/*
 * 1. Hall Sensor
 * 2. Throttle Voltage
 * 3. Gyroscope Offset
 * 4. Accelerometer Offset
 * 5. Proximity Value
 * 6. Lux Offset
 */
void setup() {
	/*Serial.begin(9600);
	// write your initialization code here
	// Initial VCNL4040 Proximity Sensor
	ProximitySensor.Setup();
	// Initial LSM6DS33 Acc & Gyroscope
	AccGyro.Setup();
	// Initial Throttle Pin
	pinMode(A0, INPUT);*/
	// write your initialization code here
	Serial.begin(9600);
	while (!Serial)
		delay(1);
	/*while (!ProximitySensor.begin()) {
		Serial.println("Couldn't find VCNL4040 chip");
		delay(10);
	}
	Serial.println("Found VCNL4040 chip");*/
	if(!apds.begin()){
		Serial.println("failed to initialize device! Please check your wiring.");
	}
	else Serial.println("Device initialized!");

	// Enable functions in APDS9960
	apds.enableProximity(true);
	// EnvLuxValueDetect();

	Serial.println("Adafruit LSM6DS33 test!");

	while (!AccGyro.begin_I2C()) {
		// if (!lsm6ds33.begin_SPI(LSM_CS)) {
		// if (!lsm6ds33.begin_SPI(LSM_CS, LSM_SCK, LSM_MISO, LSM_MOSI)) {
		Serial.println("Failed to find LSM6DS33 chip");
		delay(10);
	}
	AccGyro.configInt1(false, false, true); // accelerometer DRDY on INT1
	AccGyro.configInt2(false, true, false); // gyro DRDY on INT2
	AccGyro.getEvent(&accel, &gyro, &temp);
	accel_PrevStateX = accel.acceleration.x;
	accel_PrevStateY = accel.acceleration.y;
	accel_PrevStateZ = accel.acceleration.z;
}
void loop() {
	// Check Lock Status by Fingerprint
	/*if (FingerprintMatching()) {
		count++;
		count = count % 2;
		if(count == 1) {
			// Means the status is locked
			// Buzzer for two beeps
			isLocked = true;
		}
		else {
			// Means the status is unlocked
			// Buzzer for one beep
			isLocked = false;
		}
	}
	if (isLocked) {
		// If the e-bike is locked
		// Check the speed by Hall Sensor
		if (Hall.Monitor() < 10) {
			// If Hall Sensor is safe, check both Proximity and Acc&Gyro

		}
		else {
			theft();
		}
	}*/

	// Read Hall Sensor
    /*HallSafeState = HallSensorMonitor();
	// Set flag
	if (HallSafeState >= 10)
		flag = 1;*/

	// read Gyro & Acc
	bool isAcc = false;
	AccDifference = LSM6DS33_Algorithm();
	Serial.println(AccDifference);
	double GyroOffset = gyro_MotionState();
	// Set Flag
	/*if (AccDifference >= 2.5)
		isAcc = true;*/
	/*if (isAcc || gyro_MotionState()) {
		if (flag == 1)
			flag = 12;
		else
			flag = 2;
	}*/
	if (GyroOffset >= 2)
		EventFlag[3] = 1;
	if (AccDifference >= 2)
		EventFlag[4] = 1;

	// Check Proximity
	// uint16_t CurrentLuxValue = CalculateLux();

	// bool isLuxSafe = LuxSafeCheck(CurrentLuxValue);
	if (apds.readProximity() >=5) {
		EventFlag[5] = 1;
		Serial.println(apds.readProximity());
	}
	// Set flag
	/*if (!VCNL4040Algorithm()) {
		if (flag == 1)
			flag = 13;
		else if (flag == 12)
			flag = 123;
		else if (flag == 2)
			flag = 23;
		else
			flag = 3;
	}*/
	/*if (!isLuxSafe)
		EventFlag[6] = 1;*/

	// Check Throttle
	// Set flag
	/*if (ThrottleVoltage() >= 1.8) {
		if (flag == 1)
			flag = 14;
		else if (flag == 12)
			flag = 124;
		else if (flag == 13)
			flag = 134;
		else if (flag == 2)
			flag = 24;
		else if (flag == 23)
			flag = 234;
		else if (flag == 3)
			flag = 34;
		else
			flag = 4;
	}*/

	// Read flag
	/*switch (flag) {
		case 1:
			Serial.println("Hall Sensor");
			break;
		case 2:
			Serial.println("Gyroscope and Accelerometer");
			break;
		case 3:
			Serial.println("Proximity Sensor");
			break;
		case 4:
			Serial.println("Throttle");
			break;
		case 12:
			Serial.println("Hall Sensor and Gyroscope & Accelerometer");
			break;
		case 13:
			Serial.println("Hall Sensor and Proximity Sensor");
			break;
		case 14:
			Serial.println("Hall Sensor and Throttle");
			break;
		case 23:
			Serial.println("Gyroscope & Accelerometer and Proximity Sensor");
			break;
		case 24:
			Serial.println("Gyroscope & Accelerometer and Throttle");
			break;
		case 123:
			Serial.println("Hall Sensor, Gyroscope & Accelerometer, and Proximity Sensor");
			break;
		case 124:
			Serial.println("Hall Sensor, Gyroscope & Accelerometer, and Throttle");
			break;
		case 234:
			Serial.println("Gyroscope & Accelerometer, Proximity Sensor, and Throttle");
			break;
	}*/
	// Decision Tree
	if (EventFlag[1] == 1) {
		Serial.println("Unsafe: Hall Sensor");
		StartBuzzer();
	}
	else if (EventFlag[2] == 1) {
		Serial.println("Unsafe: Throttle");
		StartBuzzer();
	}
	else {
		if (EventFlag[3] && EventFlag[4] && EventFlag[5] && EventFlag[6] == 0)
			Serial.println("Safe");
		else {
			if (EventFlag[5] || EventFlag[6]) {
				if (EventFlag[3] || EventFlag[4]) {
					Serial.println("Unsafe: Proximity & Acc");
					StartBuzzer();
				}
				else {
					for (int x = 1; x < 20;) {
						if (EventFlag[3] && EventFlag[4] && EventFlag[5] && EventFlag[6] == 0)
							x--;
						else {
							if (EventFlag[5] || EventFlag[6]) {
								if (EventFlag[3] || EventFlag[4]) {
									Serial.println("Unsafe: Proximity & Acc");
									StartBuzzer();
									break;
								}
								x++;
							}
						}
						if (x == 20) {
							Serial.println("Unsafe: Proximity");
							StartBuzzer();
						}
					}
				}
			}
			if (EventFlag[3] || EventFlag[4]) {
				if (EventFlag[5] || EventFlag[6]) {
					Serial.println("Unsafe: Proximity & Acc");
					StartBuzzer();
				}
				else {
					for (int x = 1; x < 20;) {
						if (EventFlag[3] && EventFlag[4] && EventFlag[5] && EventFlag[6] == 0)
							x--;
						else {
							if (EventFlag[3] || EventFlag[4]) {
								if (EventFlag[5] || EventFlag[6]) {
									Serial.println("Unsafe: Proximity & Acc");
									StartBuzzer();
									break;
								}
								x++;
							}
						}
						if (x == 20) {
							Serial.println("Unsafe: Acc");
							StartBuzzer();
						}
					}
				}
			}
		}
	}
	// Clear flag
	flag = 0;
	for (int i = 0; i < 6; ++i) {
		EventFlag[i] = 0;
	}
	// AdjustEnvLux(CurrentLuxValue);
	delay(500);
	StopBuzzer();
}
