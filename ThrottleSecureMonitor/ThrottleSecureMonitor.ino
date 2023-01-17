/*
 * Read the throttle voltage
 * If the theft-action detected, reject throttle connection to the main controller
 * Otherwise, pass the throttle voltage
 */
const int THROTTLE_PIN = 13;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  // Set pin A0 to input, using for read the throttle voltage
  pinMode(A0, INPUT);
  // Set pin A1 to output, using for output the throttle voltage or 0 volt
  pinMode(A1, OUTPUT);
}

double read_throttle_voltage(){
  double throttle_voltage = analogRead(A0) * (5.0 / 1023.0) ;
  return throttle_voltage;
}

void loop() {
  // put your main code here, to run repeatedly:
  /*
   * No sensor implemented, do pass the voltage directly
   */
  double throttle_voltage = read_throttle_voltage();
  Serial.println(throttle_voltage);
  /*
  if (the_theft_action_detected) {
    return 0 volts to the main controller
  } else
  {
    directly pass the voltage to the main controller
  }*/
  // Output the throttle voltage from A1, as analog voltage
  /*
   * The
   */
  analogWrite(THROTTLE_PIN, int(throttle_voltage * 51.2));
}
