//Prateek
//www.justdoelectronics.com

#include <math.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);

#define xPin A1
#define yPin A2
#define zPin A3

#define buzzer 12

byte updateflag;

int xaxis = 0, yaxis = 0, zaxis = 0;
int deltx = 0, delty = 0, deltz = 0;
int vibration = 0, magnitude = 0, sensitivity = 45, devibrate = 75;
double angle;
unsigned long time1;

void setup() {

  Serial.begin(9600);
  lcd.begin(16, 2);
  lcd.init();
  pinMode(buzzer, OUTPUT);
  time1 = micros();
}

void loop() {
  if (micros() - time1 > 1999) Impact();

  if (updateflag > 0) {
    updateflag = 0;
    Serial.print("Impact detected!!");
    Serial.print("Magnitude:");
    Serial.print(magnitude);
    Serial.print("\t Angle:");
    Serial.print(angle, 2);
    Serial.print("\t Car Direction: NaN");
    Serial.println();
  }
}


void Impact() {

  time1 = micros();
  int oldx = xaxis;
  int oldy = yaxis;
  int oldz = zaxis;

  vibration--;
  if (vibration < 0) vibration = 0;
  xaxis = analogRead(xPin);
  yaxis = analogRead(yPin);
  zaxis = analogRead(zPin);

  if (vibration > 0) return;
  deltx = xaxis - oldx;
  delty = yaxis - oldy;
  deltz = zaxis - oldz;

  //Magnitude to calculate force of impact.
  magnitude = sqrt(sq(deltx) + sq(delty) + sq(deltz));
  if (magnitude >= sensitivity) {

    updateflag = 1;
    double X = acos((double)deltx / magnitude);
    double Y = acos((double)delty / magnitude);
    angle = (atan2(Y, X) * 180) / PI;
    angle += 180;
    vibration = devibrate;

  } else if (magnitude > 30) {
    Serial.println("Alert");
    lcd.clear();
    lcd.print(0, 1);
    lcd.print("Alert");
    digitalWrite(buzzer, HIGH);
    magnitude = 0;
  } else if (magnitude = 0) {
    lcd.clear();
    lcd.print(0, 1);
    lcd.print("Normal");
    digitalWrite(buzzer, LOW);
  }
}
