//
// Created by TedWu on 2022/4/16.
//

#ifndef MAINBOARDWIRED_EULERANGLE_H
#define MAINBOARDWIRED_EULERANGLE_H

#include "Arduino.h"
#include "Adafruit_LSM6DS33.h"
#include "Adafruit_LIS3MDL.h"
#define HP 0.7
#define LP 0.3
#define RAD2DEG 180/M_PI

Adafruit_LIS3MDL Mag;
Adafruit_LSM6DS33 AccGyro;
sensors_event_t accel, gyro, temp;
sensors_event_t gauss;

float
accelX,                 accelY,                 accelZ,            // units m/s/s i.e. accelZ if often 9.8 (gravity)
gyroX,                  gyroY,                  gyroZ,             // units dps (degrees per second)
gyroDriftX,             gyroDriftY,             gyroDriftZ,        // units dps
gyroRoll,               gyroPitch,              gyroYaw,           // units degrees (expect major drift)
gyroCorrectedRoll,      gyroCorrectedPitch,     gyroCorrectedYaw,  // units degrees (expect minor drift)
accRoll,                accPitch,               accYaw,            // units degrees (roll and pitch noisy, yaw not possible)
complementaryRoll,      complementaryPitch,     complementaryYaw,  // units degrees (excellent roll, pitch, yaw minor drift)
magX,                   magY,                   magZ,
xCom,                   yCom,                   magYaw,
pre_complementaryRoll,  pre_complementaryPitch, pre_complementaryYaw,
diff_complementaryRoll, diff_complementaryPitch, diff_complementaryYaw;

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

void doCalculations() {
  accRoll = atan2(accelY, accelZ) * RAD2DEG;
  accPitch = atan2(-accelX, sqrt(accelY * accelY + accelZ * accelZ)) * RAD2DEG;

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

  complementaryRoll = HP * (complementaryRoll + gyroCorrectedRoll) + LP * accRoll;
  complementaryPitch = HP * (complementaryPitch + gyroCorrectedPitch) + LP * accPitch;
  complementaryYaw = magYaw;

  diff_complementaryRoll = complementaryRoll - pre_complementaryRoll;
  diff_complementaryPitch = complementaryPitch - pre_complementaryPitch;
  diff_complementaryYaw = complementaryYaw - pre_complementaryYaw;
}

void printCalculations() {
  /*
  Serial.print(accRoll);
  Serial.print(',');
  Serial.print(gyroRoll);
  Serial.print(',');
  Serial.print(gyroCorrectedRoll);
  Serial.print(',');
  */
  Serial.print(complementaryRoll);
  Serial.print(',');
  Serial.print(complementaryPitch);
  Serial.print(',');
  Serial.print(magYaw);
  Serial.println("");
}

void setPrevious() {
  pre_complementaryRoll = complementaryRoll;
  pre_complementaryPitch = complementaryPitch;
  pre_complementaryYaw = complementaryYaw;
}

void IMUsetup() {
  while (!AccGyro.begin_I2C()) {
    Serial.println("Cannot find Accgyro chip");
  }
  calibrateIMU(250, 250);
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
