//
// Created by TedWu on 2022/3/13.
//

#ifndef MAINBOARD_BUZZER_H
#define MAINBOARD_BUZZER_H

#include "Arduino.h"

const int BUZZER_PIN = 2;

void StartBuzzer() {
	digitalWrite(BUZZER_PIN, HIGH);
}

void StopBuzzer() {
	digitalWrite(BUZZER_PIN, LOW);
}

#endif //MAINBOARD_BUZZER_H
