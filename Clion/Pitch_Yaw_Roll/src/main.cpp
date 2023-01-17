#include "Arduino.h"
#include "Adafruit_LSM6DS33.h"
#include "Adafruit_LIS3MDL.h"

float
gyroDriftX,        gyroDriftY,         gyroDriftZ,        // units dps
gyroRoll,          gyroPitch,          gyroYaw,           // units degrees (expect major drift)
gyroCorrectedRoll, gyroCorrectedPitch, gyroCorrectedYaw,  // units degrees (expect minor drift)
accRoll,           accPitch,           accYaw,            // units degrees (roll and pitch noisy, yaw not possible)
CF_Roll, CF_Pitch, CF_Yaw;  // units degrees (excellent roll, pitch, yaw minor drift)

Adafruit_LSM6DS33 AccGyro;
Adafruit_LIS3MDL Mag;

sensors_event_t accel;
sensors_event_t gyro;
sensors_event_t temp;

long LastTime;
long LastInterval;

bool ReadIMU () {
	// Check whether the chip is ready
	if (AccGyro.accelerationAvailable() && AccGyro.gyroscopeAvailable()) {
		Accgyro.getEvent(&accel, &gyro, &temp);
		Mag.read();
		return true;
	}
	return false;
}

void CalibrateGyro(int DelayTime, int CalibrationTime) {
	// 在每次启动的时候校准，同时也可以通过按下reset键校准
	int CalibrationCount = 0;
	delay(DelayTime);
	float sumX, sumY, sumZ;
	int startTime = millis();
	while (millis() < startTime + CalibrationTime) {
		if (ReadIMU()) {
			// Do integration
			sumX += gyro.gyro.x;
			sumY += gyro.gyro.y;
			sumZ += gyro.gyro.z;
			CalibrationCount++;
		}
	}
	if (CalibrationCount == 0)
		Serial.println("Failed to calibrate");

	gyroDriftX = sumX / CalibrationCount;
	gyroDriftY = sumY / CalibrationCount;
	gyroDriftZ = sumZ / CalibrationCount;
}

void calculation() {
	// Calculate the roll and pitch by accelerometer
	float frequency = (float) 1000000.0 / LastInterval;
	accRoll = atan2(accel.acceleration.y, accel.acceleration.z) * 180 / M_PI;
	accPitch = atan2(-accel.acceleration.x, sqrt(accel.acceleration.y * accel.acceleration.y + accel.acceleration.z * accel.acceleration.z)) * 180 / M_PI;

	/*gyroRoll += (float) gyro.gyro.x / frequency; // Simple Integration
	gyroPitch += (float) gyro.gyro.y / frequency;
	gyroYaw += (float) gyro.gyro.z / frequency;*/

	// Accelerometer & magnetometer
	float MA_x = Mag.x * cos(accPitch) + Mag.z * sin(accPitch);
	float MA_y = Mag.x * sin(accRoll) * sin(accPitch) + Mag.y * cos(accRoll) - Mag.z * sin(accRoll) * cos(accPitch);
	float M_yaw = atan2(-Mag.y, Mag.x) * 180 / M_PI;

	CF_Roll += (gyro.gyro.x - gyroDriftX) / frequency;
	CF_Pitch += (gyro.gyro.y - gyroDriftY) / frequency;
	CF_Yaw += (gyro.gyro.z - gyroDriftZ) / frequency;

	CF_Roll = 0.98 * CF_Roll + 0.02 * MA_x;
	CF_Pitch = 0.98 * CF_Pitch + 0.02 * MA_y;
	CF_Yaw = 0.98 * CF_Yaw + 0.02 * M_yaw;
}

void Algorithm() {
	if (ReadIMU()) {
		// tok
		long CurrentTime = micros();
		LastInterval = CurrentTime - LastTime;
		LastTime = CurrentTime;
		calculation();
		Serial.print("Roll: ");
		Serial.print(CF_Roll);
		Serial.print("\tPitch: ");
		Serial.print(CF_Pitch);
		Serial.print("\tYaw: ");
		Serial.print(CF_Yaw);
	}
}

void setup() {
	Serial.begin(9600);
	while (!Serial);

	while (!AccGyro.begin_I2C()) {
		// if (!lsm6ds33.begin_SPI(LSM_CS)) {
		// if (!lsm6ds33.begin_SPI(LSM_CS, LSM_SCK, LSM_MISO, LSM_MOSI)) {
		Serial.println("Failed to find LSM6DS33 chip");
	}
	while (! Mag.begin_I2C()) {          // hardware I2C mode, can pass in address & alt Wire
		//if (! lis3mdl.begin_SPI(LIS3MDL_CS)) {  // hardware SPI mode
		//if (! lis3mdl.begin_SPI(LIS3MDL_CS, LIS3MDL_CLK, LIS3MDL_MISO, LIS3MDL_MOSI)) { // soft SPI
		Serial.println("Failed to find LIS3MDL chip");
	}

	CalibrateGyro(250, 250);
	LastTime = micros();
}

void loop() {
	Algorithm();
}