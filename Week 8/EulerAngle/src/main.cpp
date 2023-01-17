//
// Created by TedWu on 2022/3/16.
//
#include "Arduino.h"
#include "Adafruit_LSM6DS33.h"
#include "Adafruit_LIS3MDL.h"

Adafruit_LIS3MDL Mag;
Adafruit_LSM6DS33 AccGyro;
sensors_event_t accel, gyro, temp;
sensors_event_t gauss;

float accelX,            accelY,             accelZ,            // units m/s/s i.e. accelZ if often 9.8 (gravity)
gyroX,             gyroY,              gyroZ,             // units dps (degrees per second)
gyroDriftX,        gyroDriftY,         gyroDriftZ,        // units dps
gyroRoll,          gyroPitch,          gyroYaw,           // units degrees (expect major drift)
gyroCorrectedRoll, gyroCorrectedPitch, gyroCorrectedYaw,  // units degrees (expect minor drift)
accRoll,           accPitch,           accYaw,            // units degrees (roll and pitch noisy, yaw not possible)
complementaryRoll, complementaryPitch, complementaryYaw,  // units degrees (excellent roll, pitch, yaw minor drift)
magX, magY, magZ,
xCom, yCom, magYaw;

long lastTime;
long lastInterval;

bool readIMU() {
	AccGyro.getEvent(&accel, &gyro, &temp);
	Mag.read();
	Mag.getEvent(&gauss);
	accelX = accel.acceleration.x;
	accelY = accel.acceleration.y;
	accelZ = accel.acceleration.z;
	gyroX = gyro.gyro.x;
	gyroY = gyro.gyro.y;
	gyroZ = gyro.gyro.z;
	magX = gauss.magnetic.x;
	magY = gauss.magnetic.y;
	magZ = gauss.magnetic.z;
	return true;
}

/*
  the gyro's x,y,z values drift by a steady amount. if we measure this when arduino is still
  we can correct the drift when doing real measurements later
*/
void calibrateIMU(int delayMillis, int calibrationMillis) {

	int calibrationCount = 0;

	delay(delayMillis); // to avoid shakes after pressing reset button

	float sumX, sumY, sumZ;
	int startTime = millis();
	while (millis() < startTime + calibrationMillis) {
		if (readIMU()) {
			// in an ideal world gyroX/Y/Z == 0, anything higher or lower represents drift
			sumX += gyroX;
			sumY += gyroY;
			sumZ += gyroZ;

			calibrationCount++;
		}
	}

	if (calibrationCount == 0) {
		Serial.println("Failed to calibrate");
	}

	gyroDriftX = sumX / calibrationCount;
	gyroDriftY = sumY / calibrationCount;
	gyroDriftZ = sumZ / calibrationCount;

}

/**
   I'm expecting, over time, the Arduino_LSM6DS3.h will add functions to do most of this,
   but as of 1.0.0 this was missing.
*/
void doCalculations() {
	accRoll = atan2(accelY, accelZ) * 180 / M_PI;
	accPitch = atan2(-accelX, sqrt(accelY * accelY + accelZ * accelZ)) * 180 / M_PI;

	float lastFrequency = (float) 1000000.0 / lastInterval;
	gyroRoll = gyroRoll + (gyroX / lastFrequency);
	gyroPitch = gyroPitch + (gyroY / lastFrequency);
	gyroYaw = gyroYaw + (gyroZ / lastFrequency);

	gyroCorrectedRoll = gyroCorrectedRoll + ((gyroX - gyroDriftX) / lastFrequency);
	gyroCorrectedPitch = gyroCorrectedPitch + ((gyroY - gyroDriftY) / lastFrequency);
	// gyroCorrectedYaw = gyroCorrectedYaw + ((gyroZ - gyroDriftZ) / lastFrequency);

	xCom = magX * cos(accPitch) + magY * sin(accPitch) - magZ * cos(accRoll) * sin(accRoll);
	yCom = magY * cos(accRoll) + magZ * sin(accRoll);
	magYaw = atan2(yCom, xCom);
	// complementaryYaw = complementaryYaw + ((gyroZ - gyroDriftZ) / lastFrequency);

	complementaryRoll = 0.9 * (complementaryRoll + gyroCorrectedRoll) + 0.1 * accRoll;
	complementaryPitch = 0.9 * (complementaryPitch + gyroCorrectedPitch) + 0.1 * accPitch;
}

/**
   This comma separated format is best 'viewed' using 'serial plotter' or processing.org client (see ./processing/RollPitchYaw3d.pde example)
*/
void printCalculations() {
	Serial.print(accRoll);
	Serial.print(',');
	Serial.print(gyroRoll);
	Serial.print(',');
	Serial.print(gyroCorrectedRoll);
	Serial.print(',');
	Serial.print(complementaryRoll);
	Serial.print(',');
	Serial.print(complementaryPitch);
	Serial.print(',');
	Serial.print(magYaw);
	Serial.println("");
}

void setup() {

	Serial.begin(9600);
	// pinMode(LED_BUILTIN, OUTPUT);

	// this sketch will wait until something connects to serial!
	// this could be 'serial monitor', 'serial plotter' or 'processing.org P3D client' (see ./processing/RollPitchYaw3d.pde file)
	while (!Serial);

	while (!AccGyro.begin_I2C()) {
		Serial.println("Cannot find Accgyro chip");
	}


	calibrateIMU(250, 250);

	lastTime = micros();

}

// the loop function runs over and over again forever
void loop() {

	if (readIMU()) {
		long currentTime = micros();
		lastInterval = currentTime - lastTime; // expecting this to be ~104Hz +- 4%
		lastTime = currentTime;

		doCalculations();
		printCalculations();

	}

}

