void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  bool wheel_ismove = false;
  int hall;
  hall = analogRead(A0);
  float motor_voltage = hall * (5.0 / 1023.0);
  if (motor_voltage <= 2.5) {       //down state
    Serial.println(motor_voltage);
    delay(10);                    //wait for upper edge
    hall = analogRead(A0);        //read voltage again
    motor_voltage = hall * (5.0 / 1023.0);
    if (motor_voltage > 2.5) {      // to the up state
      delay(10);                  //wait to drop down
      hall = analogRead(A0);        //read voltage again
      motor_voltage = hall * (5.0 / 1023.0);
      if (motor_voltage <= 2.5) {
        //means finish one cycle
        Serial.println("Wheel moved");
      }
      else;
    }
    else;
  }
  else;
}
