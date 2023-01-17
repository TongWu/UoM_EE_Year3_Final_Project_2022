//
// Created by TedWu on 2022/3/2.
//

// Integrate LSM6DS33 and LIS3MDL sensor together

#ifndef KALMAN_FILTER_KALMAN_SENSOR_H
#define KALMAN_FILTER_KALMAN_SENSOR_H

#include "Arduino.h"
#include "Adafruit_LSM6DS33.h"
// For SPI mode, we need a CS pin
#define LSM_CS 10
// For software-SPI mode we need SCK/MOSI/MISO pins
#define LSM_SCK 13
#define LSM_MISO 12
#define LSM_MOSI 11
#include "Adafruit_LIS3MDL.h"
#define LIS3MDL_CLK 13
#define LIS3MDL_MISO 12
#define LIS3MDL_MOSI 11
#define LIS3MDL_CS 10

// Kalman Sensor data structure
typedef struct{
	float acc_x, acc_y, acc_z;
	float gyro_x, gyro_y, gyro_z;
	float mag_x, mag_y, mag_z;
}kalman_data;

class Kalman_Sensor{
private:
	Adafruit_LSM6DS33 lsm6ds33;
	Adafruit_LIS3MDL lis3mdl;
public:
	Kalman_Sensor();
	void LSM6DS33_test();
	void LSM6DS33_setAcc();
	void LSM6DS33_setGyro();
	void LSM6DS33_setAccDataRate();
	void LSM6DS33_setGyroDataRate();
	void LSM6DS33_configInt();
};
#endif //KALMAN_FILTER_KALMAN_SENSOR_H
