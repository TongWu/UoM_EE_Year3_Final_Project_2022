//
// Created by TedWu on 2022/3/6.
//

#ifndef INTEGRATED_PROJECT_ACCE_GYRO_H
#define INTEGRATED_PROJECT_ACCE_GYRO_H

#include "Arduino.h"
#include "Adafruit_LSM6DS33.h"

// For SPI mode, we need a CS pin
#define LSM_CS 10
// For software-SPI mode we need SCK/MOSI/MISO pins
#define LSM_SCK 13
#define LSM_MISO 12
#define LSM_MOSI 11

class LSM6DS33{
private:
	Adafruit_LSM6DS33 AccGyro;
	float accel_CurrentStateX, accel_CurrentStateY, accel_CurrentStateZ;
	float accel_PrevStateX = 0;
	float accel_PrevStateY = 0;
	float accel_PrevStateZ = 0;
	float gyro_MotionValueX = 0;
	float gyro_MotionValueY = 0;
	float gyro_MotionValueZ = 0;
	sensors_event_t accel, gyro, temp;

public:
	float accel_CompareDifference();
	bool gyro_MotionState();
	void MotionAlgorithm();
	void Algorithm();
	void Setup();
};

#endif //INTEGRATED_PROJECT_ACCE_GYRO_H
