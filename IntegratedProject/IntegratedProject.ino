/*
 * This is the University of Manchester EEE Department Undergraduate Final year project
 * "E-bike Performance Enhancement"
 * Aims to build a security system that can be implemented on any types of bicycle - whether it has battery
 * Created by Tong Wu
 */
/*
 * Things contained:
 * 1. Arduino UNO Wi-Fi Rev.2 board
 * 2. Arduino Nano 33 IoT board
 * 3. Arduino Nano 33 BLE Sense board
 * 4. Proximity Sensors
 * 5. Accelerometers
 * 6. Gyroscopes
 * 7. Proximity Sensors
 * 8. SIM/GPS shield
 * 9. Fingerprint module
 */
/*
 * Scope of the project:
 * 1. Read values from accelerometers, gyroscopes, proximity sensors, throttle on bike and hall sensors on wheels
 * 2. Implement algorithms to monitor the status of the bike, analysis the action from bike
 * 3. Do action depends on which status has been monitored and/or analyzed
 * 4. Block any throttle voltage unless the bike has been unlocked
 */
#include <Arduino.h>
// Include self-created header file
#include "HallSensor.h"
#include "Throttle.h"

// Declare global variables:
const char READ_BACK_HALL_PIN = A0;
const char READ_THROTTLE1_PIN = A1;
const int WRITE_THROTTLE1_PIN = 13;

// Declare global class objects:


// Declare Functions:
double ReadHallSensorVoltage();     // Read hall sensor voltage from the bike
void HallSensorVoltageMonitor();    // Monitor the movement of the wheel (Single wheel usage)
// Declare classes and objects
Throttle Throttle1(READ_THROTTLE1_PIN, WRITE_THROTTLE1_PIN);
HallSensor BackHallSensor(READ_BACK_HALL_PIN);

void setup() {
  // write your initialization code here
  // Set up the communication serial with Arduino board
  Serial.begin(9600);

  // Set up Pins on Arduino boards
  // Throttle voltage monitor part:
  /* Set pin A0 to input, using for read the throttle voltage*/
  pinMode(A0, INPUT);
  /* Set pin A1 to output, using for output the throttle voltage or 0 volt */
  pinMode(A1, OUTPUT);

  Serial.println("Standby");
}

void loop() {
  // write your code here
  /*
   * TODO:
   *  Integrate the "Read throttle voltage" and "Hall sensor monitor"
   */
  /*
   * Normally, the Arduino inputs the throttle voltage, read it, and output the real throttle voltage
   * Anomaly, the Arduino inputs the throttle voltage, read it, and output zero
   */
  // Run the WheelMonitor
  BackHallSensor.HallSensorVoltageMonitor();
  // Run the ThrottleMonitor
  Throttle1.ThrottleSecureMonitor();
}







void HallSensorVoltageMonitor() {
  // Monitor the Hall sensor voltage
  /*
   * Check the voltage change with this sequence:
   * LOW -> HIGH -> LOW -> HIGH -> LOW -> HIGH -> LOW -> HIGH -> LOW -> HIGH
   */
  while(ReadHallSensorVoltage() <= 2) {
    while (ReadHallSensorVoltage() >= 4)
      while (ReadHallSensorVoltage() <= 2)
        while (ReadHallSensorVoltage() >= 4)
          while (ReadHallSensorVoltage() <= 2)
            while (ReadHallSensorVoltage() >= 4)
              while (ReadHallSensorVoltage() <= 2)
                while (ReadHallSensorVoltage() >= 4)
                  while (ReadHallSensorVoltage() <= 2)
                    while (ReadHallSensorVoltage() >= 4) {
                      Serial.println("Wheel Moved");
                      break;
                    }
  }
}
