#include <Arduino.h>
#include "DecisionAlgorithm.h"
void setup() {
  Serial.begin(9600);
  //Serial.println("----Start the security System----");
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
  // BLE.begin();
  //SIM7000C_setup();
  SIM7600CE_Setup();  // GSM & GPRS startup
  FingerprintSetup();
  // BLERecieverSetup();
  digitalWrite(BUZZER_PIN, LOW);
  vl53_Setup();
  IMUsetup();
  //displaySetup();
  RTCSetup();
  //GPSsetup();
  //JY901.StartIIC();
  getSensorData();
  delay(1000);
  getSensorData();
  Serial.println("--------Success--------");
  pinMode(2, OUTPUT);
  //SendMessage(0, 4);
}

void loop() {
  //Serial.println(getDistance());
  Algorithm();
  //JY901.GetAngle();
  //Serial.print("Angle:");Serial.print((float)JY901.stcAngle.Angle[0]/32768*180);Serial.print(" ");Serial.print((float)JY901.stcAngle.Angle[1]/32768*180);Serial.print(" ");Serial.println((float)JY901.stcAngle.Angle[2]/32768*180);
  /*
    Locker();
    getSensorData();    // Get sensor data from the main board
    // getHardwareData();  // Get hardware data from the main board
    setFlag();          // set flags according to the sensor and hardware data
    StopBuzzer();
    decisionTree();
    Clear();
  */
  //delay(50);
}
