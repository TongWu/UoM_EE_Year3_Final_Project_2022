int pin = 13;
int flag = 0;
boolean _up = false;
boolean _down = false;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  if(digitalRead(pin) == LOW)
    flag = 1;
  if( (digitalRead(pin) == HIGH) && (flag == 1)) {
    _up = true;
    flag = 0;
  }
  if (_up) {
    _up = false;
    Serial.println("Wheel moved");
  }
}
