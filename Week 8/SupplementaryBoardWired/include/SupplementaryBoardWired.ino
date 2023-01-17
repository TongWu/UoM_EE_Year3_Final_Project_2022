#include "DecisionAlgorithm.h"
void setup() {
  pinMode(OUTPUT_PIN, OUTPUT);
  Serial.begin(9600);
  vl53_Setup();
  IMUsetup();
  //displaySetup();
  JY901.StartIIC();
  getSensorData();
}

void loop() {
  Algorithm();
  /*
    getSensorData();    // Get sensor data from the main board
    // getHardwareData();  // Get hardware data from the main board
    SetFlag();          // set flags according to the sensor and hardware data
    decisionTree();
    Clear();
  */
}
