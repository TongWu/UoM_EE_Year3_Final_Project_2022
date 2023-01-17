//
// Created by TedWu on 2022/3/6.
//

#ifndef INTEGRATED_PROJECT_FINGERPRINT_H
#define INTEGRATED_PROJECT_FINGERPRINT_H
#include "Arduino.h"
#include "DFRobot_ID809_I2C.h"

#define COLLECT_NUMBER 3  //Fingerprint sampling times, can be set to 1-3
#define IRQ         6  //IRQ pin

DFRobot_ID809_I2C fingerprint;

void FingerprintReg() {
	uint8_t ID,i;
	/*Compare the captured fingerprint with all fingerprints in the fingerprint library
	  Return fingerprint ID number(1-80) if succeed, return 0 when failed
	  Function: clear the last captured fingerprint image
	 */
	fingerprint.search();       //Can add "if else" statement to judge whether the fingerprint has been registered.
	/*Get a unregistered ID for saving fingerprint
	  Return ID number when succeed
	  Return ERR_ID809 if failed
	 */
	if((ID = fingerprint.getEmptyID()) == ERR_ID809){
		while(1){
			/*Get error code imformation*/
			//desc = fingerprint.getErrorDescription();
			//Serial.println(desc);
			delay(1000);
		}
	}
	Serial.print("Unregistered ID,ID=");
	Serial.println(ID);
	i = 0;   //Clear sampling times
	/*Fingerprint Sampling 3 times */
	while(i < COLLECT_NUMBER){
		/*Set fingerprint LED ring to breathing lighting in blue*/
		fingerprint.ctrlLED(/*LEDMode = */fingerprint.eBreathing, /*LEDColor = */fingerprint.eLEDBlue, /*blinkCount = */0);
		Serial.print("The fingerprint sampling of the");
		Serial.print(i+1);
		Serial.println("(th) time is being taken");
		Serial.println("Please press down your finger");
		/*Capture fingerprint image, 10s idle timeout
		  If succeed return 0, otherwise return ERR_ID809
		 */
		if((fingerprint.collectionFingerprint(/*timeout = */10)) != ERR_ID809){
			/*Set fingerprint LED ring to quick blink in yellow 3 times*/
			fingerprint.ctrlLED(/*LEDMode = */fingerprint.eFastBlink, /*LEDColor = */fingerprint.eLEDYellow, /*blinkCount = */3);
			Serial.println("Capturing succeeds");
			i++;   //Sampling times +1
		}else{
			Serial.println("Capturing fails");
			/*Get error code information*/
			//desc = fingerprint.getErrorDescription();
			//Serial.println(desc);
		}
		Serial.println("Please release your finger");
		/*Wait for finger to release
		  Return 1 when finger is detected, otherwise return 0
		 */
		while(fingerprint.detectFinger());
	}

	/*Save fingerprint information into an unregistered ID*/
	if(fingerprint.storeFingerprint(/*Empty ID = */ID) != ERR_ID809){
		Serial.print("Saving succeed,ID=");
		Serial.println(ID);
		/*Set fingerprint LED ring to always ON in green*/
		fingerprint.ctrlLED(/*LEDMode = */fingerprint.eKeepsOn, /*LEDColor = */fingerprint.eLEDGreen, /*blinkCount = */0);
		delay(1000);
		/*Turn off fingerprint LED ring */
		fingerprint.ctrlLED(/*LEDMode = */fingerprint.eNormalClose, /*LEDColor = */fingerprint.eLEDBlue, /*blinkCount = */0);
	}else{
		Serial.println("Saving failed");
		/*Get error code information*/
		//desc = fingerprint.getErrorDescription();
		//Serial.println(desc);
	}
	Serial.println("-----------------------------");
};

bool FingerprintMatching() {
	/*Compare the captured fingerprint with all fingerprints in the fingerprint library
    Return fingerprint ID number(1-80) if succeed, return 0 when failed
   */
	bool state = false;
	uint8_t ret = fingerprint.search();
	if(ret != 0){
		/*Set fingerprint LED ring to always ON in green*/
		fingerprint.ctrlLED(/*LEDMode = */fingerprint.eKeepsOn, /*LEDColor = */fingerprint.eLEDGreen, /*blinkCount = */0);
		Serial.print("Successfully matched,ID=");
		Serial.println(ret);
		state = true;
	}else{
		/*Set fingerprint LED Ring to always ON in red*/
		fingerprint.ctrlLED(/*LEDMode = */fingerprint.eKeepsOn, /*LEDColor = */fingerprint.eLEDRed, /*blinkCount = */0);
		Serial.println("Matching failed");\
		state = false;
	}
	delay(1000);
	/*Turn off fingerprint LED Ring*/
	fingerprint.ctrlLED(/*LEDMode = */fingerprint.eNormalClose, /*LEDColor = */fingerprint.eLEDBlue, /*blinkCount = */0);
	Serial.println("-----------------------------");
	return state;
};

void FingerprintDeletion() {
	uint8_t ret;
	/*Compare the captured fingerprint with all the fingerprints in the fingerprint library
	  Return fingerprint ID(1-80) if succeed, return 0 when failed
	 */
	ret = fingerprint.search();
	if(ret){
		/*Set fingerprint LED ring to always ON in green*/
		fingerprint.ctrlLED(/*LEDMode = */fingerprint.eKeepsOn, /*LEDColor = */fingerprint.eLEDGreen, /*blinkCount = */0);
		fingerprint.delFingerprint(ret);
		Serial.print("deleted fingerprint,ID=");
		Serial.println(ret);
	}else{
		/*Set fingerprint LED ring to always ON in red*/
		fingerprint.ctrlLED(/*LEDMode = */fingerprint.eKeepsOn, /*LEDColor = */fingerprint.eLEDRed, /*blinkCount = */0);
		Serial.println("Matching failed or the fingerprint hasn't been registered");
	}
	delay(1000);
	/*Turn off fingerprint LED ring*/
	fingerprint.ctrlLED(/*LEDMode = */fingerprint.eNormalClose, /*LEDColor = */fingerprint.eLEDBlue, /*blinkCount = */0);
	Serial.println("-----------------------------");
};

#endif //INTEGRATED_PROJECT_FINGERPRINT_H
