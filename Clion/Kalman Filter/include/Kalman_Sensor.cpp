//
// Created by TedWu on 2022/3/2.
//

#include "Arduino.h"
#include "Kalman_Sensor.h"

Kalman_Sensor::Kalman_Sensor () {

}

void Kalman_Sensor::LSM6DS33_test () {
	Serial.println("Start LSM6DS33 test");

	if (!lsm6ds33.begin_I2C()) {
		// if (!lsm6ds33.begin_SPI(LSM_CS)) {
		// if (!lsm6ds33.begin_SPI(LSM_CS, LSM_SCK, LSM_MISO, LSM_MOSI)) {
		Serial.println("Failed to find LSM6DS33 chip");
		while (1) {
			delay(10);
		}
	}

	Serial.println("LSM6DS33 Found");
}

void Kalman_Sensor::LSM6DS33_setAcc () {
	// lsm6ds33.setAccelRange(LSM6DS_ACCEL_RANGE_2_G);
	Serial.print("Accelerometer range set to: ");
	switch (lsm6ds33.getAccelRange()) {
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
}

void Kalman_Sensor::LSM6DS33_setGyro () {
	// lsm6ds33.setGyroRange(LSM6DS_GYRO_RANGE_250_DPS);
	Serial.print("Gyro range set to: ");
	switch (lsm6ds33.getGyroRange()) {
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
}

void Kalman_Sensor::LSM6DS33_setAccDataRate () {
	// lsm6ds33.setAccelDataRate(LSM6DS_RATE_12_5_HZ);
	Serial.print("Accelerometer data rate set to: ");
	switch (lsm6ds33.getAccelDataRate()) {
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
}

void Kalman_Sensor::LSM6DS33_setGyroDataRate () {
	// lsm6ds33.setGyroDataRate(LSM6DS_RATE_12_5_HZ);
	Serial.print("Gyro data rate set to: ");
	switch (lsm6ds33.getGyroDataRate()) {
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
}

void Kalman_Sensor::LSM6DS33_configInt () {
	lsm6ds33.configInt1(false, false, true); // accelerometer DRDY on INT1
	lsm6ds33.configInt2(false, true, false); // gyro DRDY on INT2
}