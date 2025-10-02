#include <LiquidCrystal.h>

LiquidCrystal lcd(12, 11, 10, 9, 8, 7);

const int trigPin = 6;
const int echoPin = 5;

const int ledNear = 4;
const int ledFar = 3;
const int buzzer = 2;

const int nearThreshold = 20;
// Distance in cm to trigger near alert

long duration;
int distance;

void setup() {
  Serial.begin(9600);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(ledNear, OUTPUT);
  pinMode(ledFar, OUTPUT);
  pinMode(buzzer, OUTPUT);

  lcd.begin(16, 2);
  lcd.print("Dist Monitor");
  delay(2000);
  lcd.clear();
}

void loop() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);

  distance = duration * 0.034 / 2;

  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  lcd.setCursor(0, 0);
  lcd.print("Dist cm: ");
  lcd.print(distance);
  lcd.print("    ");

  if (distance <= nearThreshold) {
    digitalWrite(ledNear, HIGH);
    digitalWrite(ledFar, LOW);
    tone(buzzer, 1000);
  } else {
    digitalWrite(ledNear, LOW);
    digitalWrite(ledFar, HIGH);
    noTone(buzzer);
  }

  delay(1000);
}
