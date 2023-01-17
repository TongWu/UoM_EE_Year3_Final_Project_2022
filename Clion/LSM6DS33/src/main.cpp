/*
 * Basic information:
 * 1. Accelerometer measures
 * This project is in ordered to implement the accelerometer and gyroscope in LSM6DS33, to achieve functions below:
 *
 */
#include "Arduino.h"
#include <Adafruit_LSM6DS33.h>

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
	//Serial.println(accel_Difference);
	return accel_Difference;
}

bool gyro_MotionState() {
	bool isMotion = false;
	float gyro_CurrentValueX = gyro.gyro.x;
	float gyro_CurrentValueY = gyro.gyro.y;
	float gyro_CurrentValueZ = gyro.gyro.z;
	if (abs(gyro_CurrentValueX) + abs(gyro_CurrentValueY) + abs(gyro_CurrentValueZ) >= 2) {
		gyro_MotionValueX = gyro_CurrentValueX;
		gyro_MotionValueY = gyro_CurrentValueY;
		gyro_MotionValueZ = gyro_CurrentValueZ;
		isMotion = true;
	}
	return isMotion;
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

void LSM6DS33_Algorithm() {
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
	Serial.println(accel_CurrentStateX);
	float accel_difference = accel_CompareDifference();
	if(accel_difference >= 2.5) {
		// Read gyroscope
		if (gyro_MotionState()) {
			// If the sensor motion has been confirmed
			// Do the Motion Algorithm to analyze the motion type
			// MotionAlgorithm();
			// Analyzed as the stranger approached
			Serial.println("Stranger approached!");
		}
	}
}

void setup(void) {
	Serial.begin(9600);
	while (!Serial)
		delay(10); // will pause Zero, Leonardo, etc until serial console opens

	Serial.println("Adafruit LSM6DS33 test!");

	if (!AccGyro.begin_I2C()) {
		// if (!lsm6ds33.begin_SPI(LSM_CS)) {
		// if (!lsm6ds33.begin_SPI(LSM_CS, LSM_SCK, LSM_MISO, LSM_MOSI)) {
		Serial.println("Failed to find LSM6DS33 chip");
		while (1) {
			delay(10);
		}
	}

	Serial.println("LSM6DS33 Found!");

	// lsm6ds33.setAccelRange(LSM6DS_ACCEL_RANGE_2_G);
	Serial.print("Accelerometer range set to: ");
	switch (AccGyro.getAccelRange()) {
		case LSM6DS_ACCEL_RANGE_2_G:
			Serial.println("+-2G");
			break;
		case LSM6DS_ACCEL_RANGE_4_G:
			Serial.println("+-4G");
			break;
		case LSM6DS_ACCEL_RANGE_8_G:
			Serial.println("+-8G");
			break;
		case LSM6DS_ACCEL_RANGE_16_G:
			Serial.println("+-16G");
			break;
	}

	// lsm6ds33.setGyroRange(LSM6DS_GYRO_RANGE_250_DPS);
	Serial.print("Gyro range set to: ");
	switch (AccGyro.getGyroRange()) {
		case LSM6DS_GYRO_RANGE_125_DPS:
			Serial.println("125 degrees/s");
			break;
		case LSM6DS_GYRO_RANGE_250_DPS:
			Serial.println("250 degrees/s");
			break;
		case LSM6DS_GYRO_RANGE_500_DPS:
			Serial.println("500 degrees/s");
			break;
		case LSM6DS_GYRO_RANGE_1000_DPS:
			Serial.println("1000 degrees/s");
			break;
		case LSM6DS_GYRO_RANGE_2000_DPS:
			Serial.println("2000 degrees/s");
			break;
		case ISM330DHCX_GYRO_RANGE_4000_DPS:
			break; // unsupported range for the DS33
	}

	// lsm6ds33.setAccelDataRate(LSM6DS_RATE_12_5_HZ);
	Serial.print("Accelerometer data rate set to: ");
	switch (AccGyro.getAccelDataRate()) {
		case LSM6DS_RATE_SHUTDOWN:
			Serial.println("0 Hz");
			break;
		case LSM6DS_RATE_12_5_HZ:
			Serial.println("12.5 Hz");
			break;
		case LSM6DS_RATE_26_HZ:
			Serial.println("26 Hz");
			break;
		case LSM6DS_RATE_52_HZ:
			Serial.println("52 Hz");
			break;
		case LSM6DS_RATE_104_HZ:
			Serial.println("104 Hz");
			break;
		case LSM6DS_RATE_208_HZ:
			Serial.println("208 Hz");
			break;
		case LSM6DS_RATE_416_HZ:
			Serial.println("416 Hz");
			break;
		case LSM6DS_RATE_833_HZ:
			Serial.println("833 Hz");
			break;
		case LSM6DS_RATE_1_66K_HZ:
			Serial.println("1.66 KHz");
			break;
		case LSM6DS_RATE_3_33K_HZ:
			Serial.println("3.33 KHz");
			break;
		case LSM6DS_RATE_6_66K_HZ:
			Serial.println("6.66 KHz");
			break;
	}

	// lsm6ds33.setGyroDataRate(LSM6DS_RATE_12_5_HZ);
	Serial.print("Gyro data rate set to: ");
	switch (AccGyro.getGyroDataRate()) {
		case LSM6DS_RATE_SHUTDOWN:
			Serial.println("0 Hz");
			break;
		case LSM6DS_RATE_12_5_HZ:
			Serial.println("12.5 Hz");
			break;
		case LSM6DS_RATE_26_HZ:
			Serial.println("26 Hz");
			break;
		case LSM6DS_RATE_52_HZ:
			Serial.println("52 Hz");
			break;
		case LSM6DS_RATE_104_HZ:
			Serial.println("104 Hz");
			break;
		case LSM6DS_RATE_208_HZ:
			Serial.println("208 Hz");
			break;
		case LSM6DS_RATE_416_HZ:
			Serial.println("416 Hz");
			break;
		case LSM6DS_RATE_833_HZ:
			Serial.println("833 Hz");
			break;
		case LSM6DS_RATE_1_66K_HZ:
			Serial.println("1.66 KHz");
			break;
		case LSM6DS_RATE_3_33K_HZ:
			Serial.println("3.33 KHz");
			break;
		case LSM6DS_RATE_6_66K_HZ:
			Serial.println("6.66 KHz");
			break;
	}

	AccGyro.configInt1(false, false, true); // accelerometer DRDY on INT1
	AccGyro.configInt2(false, true, false); // gyro DRDY on INT2
	AccGyro.getEvent(&accel, &gyro, &temp);
	accel_PrevStateX = accel.acceleration.x;
	accel_PrevStateY = accel.acceleration.y;
	accel_PrevStateZ = accel.acceleration.z;
}

void loop() {
	//  /* Get a new normalized sensor event */
	/*sensors_event_t accel;
	sensors_event_t gyro;
	sensors_event_t temp;
	lsm6ds33.getEvent(&accel, &gyro, &temp);*/

	/*Serial.print("\t\tTemperature ");
	Serial.print(temp.temperature);
	Serial.println(" deg C");

	*//* Display the results (acceleration is measured in m/s^2) *//*
	Serial.print("\t\tAccel X: ");
	Serial.print(accel.acceleration.x);
	Serial.print(" \tY: ");
	Serial.print(accel.acceleration.y);
	Serial.print(" \tZ: ");
	Serial.print(accel.acceleration.z);
	Serial.println(" m/s^2 ");

	*//* Display the results (rotation is measured in rad/s) *//*
	Serial.print("\t\tGyro X: ");
	Serial.print(gyro.gyro.x);
	Serial.print(" \tY: ");
	Serial.print(gyro.gyro.y);
	Serial.print(" \tZ: ");
	Serial.print(gyro.gyro.z);
	Serial.println(" radians/s ");
	Serial.println();*/

	LSM6DS33_Algorithm();
	delay(500);

	//  // serial plotter friendly format

	//  Serial.print(temp.temperature);
	//  Serial.print(",");

	//  Serial.print(accel.acceleration.x);
	//  Serial.print(","); Serial.print(accel.acceleration.y);
	//  Serial.print(","); Serial.print(accel.acceleration.z);
	//  Serial.print(",");

	// Serial.print(gyro.gyro.x);
	// Serial.print(","); Serial.print(gyro.gyro.y);
	// Serial.print(","); Serial.print(gyro.gyro.z);
	// Serial.println();
	//  delayMicroseconds(10000);
}