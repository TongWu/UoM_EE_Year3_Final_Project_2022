#include <Arduino.h>
#include <Adafruit_LSM6DS33.h>
#include "Adafruit_LIS3MDL.h"
#include "Kalman.h"
using namespace BLA;

/*KALMAN PART*/
#define Nstate 3 // position, speed, acceleration
#define Nobs 3   // position, acceleration
// measurement std of the noise
#define n_p 0.3 // position measurement noise
#define n_a 5.0 // acceleration measurement noise
// model std (1/inertia)
#define m_p 0.1
#define m_s 0.1
#define m_a 0.8
BLA::Matrix<Nobs> obs; // observation vector
KALMAN<Nstate,Nobs> K; // your Kalman filter
unsigned long T; // current time
float DT; // delay between two updates of the filter
BLA::Matrix<Nstate> state; // true state vector
#define SIMUL_PERIOD 0.3 // oscillating period [s]
#define SIMUL_AMP 1.0    // oscillation amplitude
#define LOOP_DELAY 10    // add delay in the measurement loop [ms]

// For SPI mode, we need a CS pin
#define LSM_CS 10
// For software-SPI mode we need SCK/MOSI/MISO pins
#define LSM_SCK 13
#define LSM_MISO 12
#define LSM_MOSI 11
#define G 9.80665
// Define sample time in ms
#define SAMPLE_TIME 300
#define GYRO_OFFSET_X 0.0477
#define GYRO_OFFSET_Y (-0.0635)
#define GYRO_OFFSET_Z (-0.0291)

Adafruit_LSM6DS33 Acc_Gyro;
sensors_event_t accel, gyro, temp;
bool isBootUp = false;
// Previous parameters
/*double Prev_RxEst = 0;
double Prev_RyEst = 0;
double Prev_RzEst = 0;
double Prev_Ayz = 0;
double Prev_Axz = 0;
double Prev_Axy = 0;
double Prev_RateYZ = 0;
double Prev_RateXZ = 0;
double Prev_RateXY = 0;
auto TrustGyro = 5;*/

/*int sign(double input) {
	int result = 0;
	if (input >= 0)
		result = 1;
	else result = 0;
	return result;
}*/

void SIMULATOR_INIT(){
	// Initialize stuff for the simulator
	//randomSeed(analogRead(0));
	/*
	 * To measure the sensor
	 */
	Acc_Gyro.getEvent(&accel, &gyro, &temp);
	K.F = {accel.acceleration.x,gyro.gyro.x,temp.temperature,
		         accel.acceleration.y,gyro.gyro.y,temp.temperature,
				 accel.acceleration.z,gyro.gyro.z,temp.temperature};

	state.Fill(0.0);
	obs.Fill(0.0);
}

void SIMULATOR_UPDATE(){
	// Simulate a physical process
	// Here we simulate a sinusoidal position of an object
	unsigned long tcur = millis();
	state(0) = SIMUL_AMP*sin(tcur/1000.0/SIMUL_PERIOD); // position
	state(1) = SIMUL_AMP/SIMUL_PERIOD*cos(tcur/1000.0/SIMUL_PERIOD); // speed
	state(2) = -SIMUL_AMP/SIMUL_PERIOD/SIMUL_PERIOD*sin(tcur/1000.0/SIMUL_PERIOD); // acceleration
}

double SIMULATOR_GAUSS_NOISE(){
	// Generate centered reduced Gaussian random number with Box-Muller algorithm
	double u1 = random(1,10000)/10000.0;
	double u2 = random(1,10000)/10000.0;
	return sqrt(-2*log(u1))*cos(2*M_PI*u2);
}

void SIMULATOR_MEASURE(){
	// Simulate a noisy measurement of the physical process
	BLA::Matrix<Nobs> noise;
	noise(0) = n_p * SIMULATOR_GAUSS_NOISE();
	noise(1) = n_a * SIMULATOR_GAUSS_NOISE();
	obs = K.H * state + noise; // measurement
	delay(LOOP_DELAY); //simulate a delay in the measurement
}



void AccAlgorithm() {
	// Initialise Parameters
	// Accelerometer part
	// 3-axis value of accelerometer, in g
	double RxAcc, RyAcc, RzAcc;
	// Array to contain three value mentioned
	double RAcc[3] = {0};
	// Unit length of 3-axis value of accelerometer
	double uintRAcc;
	// Normalised 3-axis value of accelerometer
	double RAccNormalised[3] = {0};
	// Accelerometer Part
	Acc_Gyro.getEvent(&accel, &gyro, &temp);
	RxAcc = accel.acceleration.x / G;
	RyAcc = accel.acceleration.y / G;
	RzAcc = accel.acceleration.z / G;
	RAcc[0] = RxAcc;
	RAcc[1] = RyAcc;
	RAcc[2] = RzAcc;
	uintRAcc = sqrt(RxAcc*RxAcc + RyAcc*RyAcc + RzAcc*RzAcc);
	RAccNormalised[0] = RxAcc / uintRAcc;
	RAccNormalised[1] = RyAcc / uintRAcc;
	RAccNormalised[2] = RzAcc / uintRAcc;
}

void GyroAlgorithm() {
	/*
	 * 在速度计测算出每个轴上的重力之后，通过陀螺仪判断是否有剧烈的移动。
	 * 并且通过速度计和陀螺仪判断是否有
	 */
	double GyroX, GyroY, GyroZ;
	Acc_Gyro.getEvent(&accel, &gyro, &temp);
	GyroX = gyro.gyro.x;
	GyroY = gyro.gyro.y;
	GyroZ = gyro.gyro.z;

}

void GyroSupportAlgorithm() {
	/*
	 * This algorithm should compute the 3-axis accelerometer values by 3-axis gyroscope
	 * It is totally possible, while the precision of the gyroscope of this sensor is too low to calculate
	 * So the algorithm has been diminished
	 */
/*	// Gyroscope Part
	// 3axis value of gyroscope, in deg/s
	double RateAyzGyro, RateAxzGyro, RateAxyGyro;
	// Change rate of angle between x-axis and z-axis; y-axis and z-axis, in deg/s
	double RateAyzAvg, RateAxzAvg, RateAxyAvg;
	double Ayz, Axz, Axy;
	double unitRGyro;
	double RxGyro, RyGyro, RzGyro;
	double RGyro[3] = {0};
	double RxEst, RyEst, RzEst;
	double R;
    Acc_Gyro.getEvent(&accel, &gyro, &temp);
	RateAyzGyro = (gyro.gyro.x - GYRO_OFFSET_X) ;
	RateAxzGyro = (gyro.gyro.y - GYRO_OFFSET_Y) ;
	RateAxyGyro = (gyro.gyro.z - GYRO_OFFSET_Z) ;
	RateAyzAvg = (RateAyzGyro + Prev_RateYZ) / 2;
	RateAxzAvg = (RateAxzGyro + Prev_RateXZ) / 2;
	RateAxyAvg = (RateAxyGyro + Prev_RateXY) / 2;
	Ayz = Prev_Ayz + RateAyzAvg * SAMPLE_TIME/1000;
	Axz = Prev_Axz + RateAxzAvg * SAMPLE_TIME/1000;
	Axy = Prev_Axy + RateAxyAvg * SAMPLE_TIME/1000;
	unitRGyro = 1;
	// RxGyro = 1/sqrt(1 + cot(Ax)^2 + sec(Ay)^2 )
	RxGyro = 1 / sqrt(1 + (1/tan(Axz)) * (1/tan(Axz)) * (1/cos(Ayz)) * (1/cos(Ayz)) );
	RyGyro = sin(Ayz) / sqrt(1 + cos(Ayz) * cos(Ayz) * tan(Axz) * tan(Axz));
	if (abs(Prev_RzEst) <= 0.005) {
		RxEst = Prev_RxEst;
		RyEst = Prev_RyEst;
		RzEst = Prev_RzEst;
	} else{
		RzGyro = sign(Prev_RzEst) * sqrt(1 - RxGyro*RxGyro - RyGyro*RyGyro);
		RGyro[0] = RxGyro;
		RGyro[1] = RyGyro;
		RGyro[2] = RzGyro;
		RxEst = (RxAcc + RxGyro * TrustGyro) / (1 + TrustGyro);
		RyEst = (RyAcc + RyGyro * TrustGyro) / (1 + TrustGyro);
		RzEst = (RzAcc + RzGyro * TrustGyro) / (1 + TrustGyro);
		R = sqrt(pow(RxEst, 2) + pow(RyEst, 2) + pow(RzEst, 2));
		RxEst /= R;
		RyEst /= R;
		RzEst /= R;
	}
	// Store the current value to previous
	Prev_RxEst = RxEst;
	Prev_RyEst = RyEst;
	Prev_RzEst = RzEst;
	Prev_Ayz = Ayz;
	Prev_Axz = Axz;
	Prev_Axy = Axy;
	Prev_RateYZ = RateAyzGyro;
	Prev_RateXZ = RateAxzGyro;
	Prev_RateXY = RateAxyGyro;*/
}

// To make a fusion between Accelerometer and Gyroscope
void FusionAlgorithm() {
	// Initialise Parameters
	// Accelerometer part
		// 3-axis value of accelerometer, in g
		double RxAcc, RyAcc, RzAcc;
		// Array to contain three value mentioned
		double RAcc[3] = {0};
		// Unit length of 3-axis value of accelerometer
		double uintRAcc;
		// Normalised 3-axis value of accelerometer
		double RAccNormalised[3] = {0};
	// Accelerometer Part
	Acc_Gyro.getEvent(&accel, &gyro, &temp);
	RxAcc = accel.acceleration.x / G;
	RyAcc = accel.acceleration.y / G;
	RzAcc = accel.acceleration.z / G;
	RAcc[0] = RxAcc;
	RAcc[1] = RyAcc;
	RAcc[2] = RzAcc;
	uintRAcc = sqrt(RxAcc*RxAcc + RyAcc*RyAcc + RzAcc*RzAcc);
	RAccNormalised[0] = RxAcc / uintRAcc;
	RAccNormalised[1] = RyAcc / uintRAcc;
	RAccNormalised[2] = RzAcc / uintRAcc;

	Serial.print("Normalised x:\t");
	Serial.print(RAccNormalised[0]);
	Serial.print(" y:\t");
	Serial.print(RAccNormalised[1]);
	Serial.print(" z:\t");
	Serial.println(RAccNormalised[2]);
}

void setup(void) {
	Serial.begin(9600);
	while (!Serial)
		delay(10); // will pause Zero, Leonardo, etc until serial console opens

	Serial.println("Sensor test");
	while (!Acc_Gyro.begin_I2C()) {
		Serial.println("Failed to find LSM6DS33 chip");
		delay(10);
	}

	// time evolution matrix (whatever... it will be updated inloop)
	K.F = {1.0, 0.0, 0.0,
	       0.0, 1.0, 0.0,
	       0.0, 0.0, 1.0};

	// measurement matrix n the position (e.g. GPS) and acceleration (e.g. accelerometer)
	K.H = {1.0, 0.0, 0.0,
	       0.0, 0.0, 1.0};
	// measurement covariance matrix
	K.R = {n_p*n_p,   0.0,
	       0.0, n_a*n_a};
	// model covariance matrix
	K.Q = {m_p*m_p,     0.0,     0.0,
	       0.0, m_s*m_s,     0.0,
	       0.0,     0.0, m_a*m_a};

	T = millis();

	// INITIALIZE SIMULATION
	SIMULATOR_INIT();
}

void loop() {
	// write your code here
	FusionAlgorithm();
}

