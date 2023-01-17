#include "Arduino.h"

double ReadHallSensorVoltage(){
	double HallSensorVoltage = analogRead(A0) * (5.0 / 1023.0);
	return HallSensorVoltage;
}

class HallSensor {
private:
public:
	HallSensor() = default;;
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
};
HallSensor BackHallSensor;

void setup() {
	// put your setup code here, to run once:
	Serial.begin(9600);
	Serial.println("Standby");
}

void loop() {
	/*
	Arduino change the analog voltage to 0-1023;
	Change it again to 0-5V
	The boot voltage is 0.83V*/
	// double HallSensorVoltage = ReadHallSensorVoltage();
	// Serial.println(throttle_voltage);
	Serial.println("Standby");
	/*
	The hall voltage is 0V or 5V, so if voltage=0V, wait the voltage jump to 5V, then wait again to 0V
	To complete the one single wheel move*/
	/*
	while(read_throttle_voltage() <= 2){
		// Serial.println("Hall in LOW, standby");
		while(read_throttle_voltage() >= 4){
			// Serial.println("Hall in HIGH, standby");
			while(read_throttle_voltage() <= 2){
				// Serial.println("!!!WHEEL MOVED!!!");
				break;
			}

		}
	}*/
	BackHallSensor.HallSensorVoltageMonitor();
}