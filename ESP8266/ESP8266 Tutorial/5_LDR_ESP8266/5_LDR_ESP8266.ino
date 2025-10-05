//Code 1

void setup(){
  Serial.begin(115200);
}

void loop(){
  int analog = analogRead(A0);
  float volt = analog * (3.3 / 1023.0);

  Serial.print("Analog: ");
  Serial.print(analog);
  Serial.print(" / Voltage: ");
  Serial.println(volt);
  delay(1000);  
}


/*

Code 2

void setup() {
  Serial.begin(115200);
}
void loop() {
  int analog = analogRead(A0);
  float volt = analog * (3.3 / 1023.0);
  Serial.print("Value: ");
  Serial.println(analog);

  if (analog < 150) {
    Serial.println("Night");
  } else {
    Serial.println("Day");
  }
  delay(100);
}

*/

