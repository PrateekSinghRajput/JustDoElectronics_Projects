#include <LiquidCrystal.h>

LiquidCrystal lcd(12, 11, 10, 9, 8, 7);

const int soilMoisturePin = A0;
const int ledDry = 6;
const int ledWet = 5;
const int buzzer = 4;

const int dryThreshold = 700;
const int wetThreshold = 300;

void setup() {
  Serial.begin(9600);

  pinMode(ledDry, OUTPUT);
  pinMode(ledWet, OUTPUT);
  pinMode(buzzer, OUTPUT);

  lcd.begin(16, 2);
  lcd.print("Soil Moisture");
  delay(2000);
  lcd.clear();
}

void loop() {
  int sensorValue = analogRead(soilMoisturePin);

  Serial.print("Soil Moisture Value: ");
  Serial.println(sensorValue);

  lcd.setCursor(0, 0);
  lcd.print("Moisture Val:");
  lcd.setCursor(0, 1);
  lcd.print(sensorValue);
  lcd.print("     ");

  if (sensorValue >= dryThreshold) {
    digitalWrite(ledDry, HIGH);
    digitalWrite(ledWet, LOW);
    tone(buzzer, 1000);
  } else if (sensorValue <= wetThreshold) {
    digitalWrite(ledDry, LOW);
    digitalWrite(ledWet, HIGH);
    noTone(buzzer);
  } else {
    digitalWrite(ledDry, LOW);
    digitalWrite(ledWet, LOW);
    noTone(buzzer);
  }

  delay(1000);
}
