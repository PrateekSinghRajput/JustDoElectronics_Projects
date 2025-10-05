#include <Servo.h>
Servo servo;

void setup() {
  servo.attach(0);  //D3
}

void loop() {
  servo.write(0);
  delay(1000);
  servo.write(180);
  delay(1000);
}