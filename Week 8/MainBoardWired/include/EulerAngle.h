//
// Created by TedWu on 2022/4/16.
//

#ifndef MAINBOARDWIRED_EULERANGLE_H
#define MAINBOARDWIRED_EULERANGLE_H

#include "Arduino.h"
#include "Adafruit_LSM6DS33.h"
#include "Adafruit_LIS3MDL.h"
#define HP 0.6
#define LP 0.4
#define RADIAN_DEGREE 180/M_PI

Adafruit_LIS3MDL Mag;
Adafruit_LSM6DS33 AccGyro;
sensors_event_t accel, gyro, temp;

float accelX, accelY, accelZ;
float gyroscope_x, gyroscope_y, gyroscope_z;
float drift_x, drift_y, drift_z;
float gyroscope_roll, gyroscope_pitch, gyroscope_yaw;
float gyroscope_afterDrift_Roll, gyroscope_afterDrift_Pitch, gyroscope_afterDrift_Yaw;
float accelerometer_roll, accelerometer_pitch;
float complementaryFilter_roll, complementaryFilter_pitch, complementaryFilter_yaw;
float magnetometer_x, magnetometer_y, magnetometer_z;
float acc_mag_CF_x, acc_mag_CF_y, magnetometer_yaw;
float pre_complementaryRoll, pre_complementaryPitch, pre_complementaryYaw;
float diff_complementaryRoll, diff_complementaryPitch, diff_complementaryYaw;

long lastTime;
long lastInterval;

bool readIMU() {
	AccGyro.getEvent(&accel, &gyro, &temp);
	Mag.read();
	accelX = accel.acceleration.x;
	accelY = accel.acceleration.y;
	accelZ = accel.acceleration.z;
	gyroscope_x = gyro.gyro.x;
	gyroscope_y = gyro.gyro.y;
	gyroscope_z = gyro.gyro.z;
	magnetometer_x = Mag.x;
	magnetometer_y = Mag.y;
	magnetometer_z = Mag.z;
	return true;
}

void calibrate(int delayTime, int calibrationTime) {
	int calibrationCount = 0;
	delay(delayTime);
	float x, y, z;
	int startTime = millis();
	while (millis() < startTime + calibrationTime) {
		if (readIMU()) {
			x += gyroscope_x;
			y += gyroscope_y;
			z += gyroscope_z;
			calibrationCount++;
		}
	}

	if (calibrationCount == 0) {
		Serial.println("Failed to calibrate");
	}

	drift_x = x / calibrationCount;
	drift_y = y / calibrationCount;
	drift_z = z / calibrationCount;

}

void doCalculations() {
	accelerometer_roll = atan2(accelY, accelZ) * RADIAN_DEGREE;
	accelerometer_pitch = atan2(-accelX, sqrt(accelY * accelY + accelZ * accelZ)) * RADIAN_DEGREE;

	float lastFrequency = (float) 1000000.0 / lastInterval;
	gyroscope_roll = gyroscope_roll + (gyroscope_x / lastFrequency);
	gyroscope_pitch = gyroscope_pitch + (gyroscope_y / lastFrequency);
	gyroscope_yaw = gyroscope_yaw + (gyroscope_z / lastFrequency);

	gyroscope_afterDrift_Roll = gyroscope_afterDrift_Roll + ((gyroscope_x - drift_x) / lastFrequency);
	gyroscope_afterDrift_Pitch = gyroscope_afterDrift_Pitch + ((gyroscope_y - drift_y) / lastFrequency);
	// gyroscope_afterDrift_Yaw = gyroscope_afterDrift_Yaw + ((gyroscope_z - drift_z) / lastFrequency);

	acc_mag_CF_x = magnetometer_x * cos(accelerometer_pitch) + magnetometer_y * sin(accelerometer_pitch) - magnetometer_z * cos(accelerometer_roll) * sin(accelerometer_roll);
	acc_mag_CF_y = magnetometer_y * cos(accelerometer_roll) + magnetometer_z * sin(accelerometer_roll);
	magnetometer_yaw = atan2(acc_mag_CF_y, acc_mag_CF_x);
	complementaryFilter_yaw = complementaryFilter_yaw + ((gyroscope_z - drift_z) / lastFrequency);

	complementaryFilter_roll = HP * (complementaryFilter_roll + gyroscope_afterDrift_Roll) + LP * accelerometer_roll;
	complementaryFilter_pitch = HP * (complementaryFilter_pitch + gyroscope_afterDrift_Pitch) + LP * accelerometer_pitch;
	//complementaryFilter_yaw = magnetometer_yaw;

	diff_complementaryRoll = complementaryFilter_roll - pre_complementaryRoll;
	diff_complementaryPitch = complementaryFilter_pitch - pre_complementaryPitch;
	diff_complementaryYaw = complementaryFilter_yaw - pre_complementaryYaw;
}

void printCalculations() {
  Serial.print("Roll: ");
	Serial.print(complementaryFilter_roll);
	Serial.print(',');
  Serial.print("Pitch: ");
	Serial.print(complementaryFilter_pitch);
	Serial.print(',');
  Serial.print("Yaw: ");
	Serial.print(magnetometer_yaw);
  Serial.print(',');
	//Serial.println("");
}

void setPrevious() {
	pre_complementaryRoll = complementaryFilter_roll;
	pre_complementaryPitch = complementaryFilter_pitch;
	pre_complementaryYaw = complementaryFilter_yaw;
}

void IMUsetup() {
	while (!AccGyro.begin_I2C()) {
		Serial.println("Cannot find Accgyro chip");
	}
 while (!Mag.begin_I2C()) {
  Serial.println("Cannot find Mag chip");
 }
	calibrate(250, 250);
	lastTime = micros();
}

void getEulerAngle() {
	if (readIMU()) {
		setPrevious();
		long currentTime = micros();
		lastInterval = currentTime - lastTime;
		lastTime = currentTime;
		doCalculations();
		//printCalculations();
	}
}

#endif //MAINBOARDWIRED_EULERANGLE_H
