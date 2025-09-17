//Prateek
//www.justdoelectronics.com

const int x = A0;
const int y = A1;
const int z = A2;

int xv = 0, yv = 0, zv = 0;

#define buzzer 12

void setup() {

  Serial.begin(9600);
  Serial.println("Serial.begin");
  pinMode(buzzer, OUTPUT);
  delay(2000);
}

void loop() {
  adxl();
  delay(1000);
}

void adxl() {
  xv = analogRead(x);
  yv = analogRead(y);
  zv = analogRead(z);
  Serial.print("x= ");
  Serial.print(xv);
  Serial.print(" y= ");
  Serial.print(yv);
  Serial.print(" z= ");
  Serial.println(zv);

  if ((xv < 280) || (xv > 380) || (yv < 280) || (yv > 380)) {
    Serial.println("Accident happen");
    digitalWrite(buzzer, HIGH);
  } else {
    Serial.println("Normal");
    digitalWrite(buzzer, LOW);
  }
}
