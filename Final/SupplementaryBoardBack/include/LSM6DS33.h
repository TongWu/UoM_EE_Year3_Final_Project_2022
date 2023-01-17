//
// Created by TedWu on 2022/3/13.
//

#ifndef MAINBOARD_LSM6DS33_H
#define MAINBOARD_LSM6DS33_H

#include "Adafruit_LSM6DS33.h"
// For SPI mode, we need a CS pin
#define LSM_CS 10
// For software-SPI mode we need SCK/MOSI/MISO pins
#define LSM_SCK 13
#define LSM_MISO 12
#define LSM_MOSI 11

// Create object for LSM6DS33
Adafruit_LSM6DS33 AccGyro;

// Define statement of the gyro and accelerometer
float accel_PrevStateX = 0;
float accel_PrevStateY = 0;
float accel_PrevStateZ = 0;

// Create event object
sensors_event_t accel, gyro, temp;

void LSM6DS33_Setup() {
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
	Serial.println("Acc & Gyro Ready");
}

/*
 * Calculate the difference for each axis of the accelerometer, then sum them up to get the accelerometer difference
 * return: (float) accelerometer difference
 */
float AccelCalculateDifference() {
	float accel_Difference = abs(accel.acceleration.x - accel_PrevStateX) + abs(accel.acceleration.y - accel_PrevStateY) +
	                         abs(accel.acceleration.z - accel_PrevStateZ);
	accel_PrevStateX = accel.acceleration.x;
	accel_PrevStateY = accel.acceleration.y;
	accel_PrevStateZ = accel.acceleration.z;
	//Serial.println(accel_Difference);
	return accel_Difference;
}

float getGyro() {
	/*if (abs(gyro_CurrentValueX) + abs(gyro_CurrentValueY) + abs(gyro_CurrentValueZ) >= 2) {
		gyro_MotionValueX = gyro_CurrentValueX;
		gyro_MotionValueY = gyro_CurrentValueY;
		gyro_MotionValueZ = gyro_CurrentValueZ;
		isMotion = true;
	}*/
	return abs(gyro.gyro.x) + abs(gyro.gyro.y) + abs(gyro.gyro.z);
}

void LSM6DS33() {
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
	// Serial.println(accel_CurrentStateX);
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
}
#endif //MAINBOARD_LSM6DS33_H
