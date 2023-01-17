/*!
 * @file  SIM808_CallUp.ino
 * @brief  Make phone calls
 * @details  1. This example is used to test DFRobot_SIM808 SIM808 GPS/GPRS/GSM Shield's callUp function.
 * @n        2. Open the SIM808_CallUp example or copy these code to your project
 * @n        3. Replace the phoneNumber：#define PHONE_NUMBER  "187******39"
 * @n        4. Download and dial the function switch to Arduino, and enjoy it
 * @copyright  Copyright (c) 2010 DFRobot Co.Ltd (http://www.dfrobot.com)
 * @license  The MIT License (MIT)
 * @author  [Jason](jason.ling@dfrobot.com)
 * @maintainer  [qsjhyy](yihuan.huang@dfrobot.com)
 * @version  V1.0
 * @date  2022-02-08
 * @url  https://github.com/DFRobot/DFRobot_SIM808
 */
#include <DFRobot_SIM808.h>
#include <SoftwareSerial.h>

//#define PIN_TX    10
//#define PIN_RX    11
//SoftwareSerial mySerial(PIN_TX,PIN_RX);
//DFRobot_SIM808 sim808(&mySerial);//Connect RX,TX,PWR,

//Mobile phone number,need to change
#define PHONE_NUMBER  "187******39"   

DFRobot_SIM808 sim808(&Serial);

void setup() {
  //mySerial.begin(9600);
  Serial.begin(9600);
  //********Initialize sim808 module*************
  while(!sim808.init()) { 
      delay(1000);
      Serial.print("Sim808 init error\r\n");
  }
  Serial.println("Sim808 init success");
  Serial.println("Start to call ...");

  //*********Call specified number***************
  sim808.callUp(PHONE_NUMBER);
}
void loop() {
  //nothing to do
}
