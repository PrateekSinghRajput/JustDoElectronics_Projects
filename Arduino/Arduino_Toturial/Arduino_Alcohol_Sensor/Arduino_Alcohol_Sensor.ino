#include <LiquidCrystal.h>

LiquidCrystal lcd(12, 11, 10, 9, 8, 7);

const int mq3AnalogPin = A0;
const int ledHigh = 6;
const int ledLow = 5;
const int buzzer = 4;

const int highAlcoholThreshold = 400;
const int lowAlcoholThreshold = 150;

void setup() {
  Serial.begin(9600);

  pinMode(ledHigh, OUTPUT);
  pinMode(ledLow, OUTPUT);
  pinMode(buzzer, OUTPUT);

  lcd.begin(16, 2);
  lcd.print("Alcohol Sensor");
  delay(2000);
  lcd.clear();
}

void loop() {
  int sensorValue = analogRead(mq3AnalogPin);

  Serial.print("MQ3 Sensor Value: ");
  Serial.println(sensorValue);

  lcd.setCursor(0, 0);
  lcd.print("Alcohol Val:");
  lcd.setCursor(0, 1);
  lcd.print(sensorValue);
  lcd.print("     ");

  if (sensorValue >= highAlcoholThreshold) {
    digitalWrite(ledHigh, HIGH);
    digitalWrite(ledLow, LOW);
    tone(buzzer, 1000);
  } else if (sensorValue >= lowAlcoholThreshold) {
    digitalWrite(ledHigh, LOW);
    digitalWrite(ledLow, HIGH);
    noTone(buzzer);
  } else {
    digitalWrite(ledHigh, LOW);
    digitalWrite(ledLow, LOW);
    noTone(buzzer);
  }

  delay(1000);
}
