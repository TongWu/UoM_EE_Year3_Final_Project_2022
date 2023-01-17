void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  // Read throttle input voltage in pin 0
  int throttle = analogRead(A0);
  
  // Arduino change the analog voltage to 0-1023;
  // Change it again to 0-5V
  // The boot voltage is 0.83V
  
  float throttle_voltage = throttle * (5.0 / 1023.0);

  Serial.println(throttle_voltage);
}
