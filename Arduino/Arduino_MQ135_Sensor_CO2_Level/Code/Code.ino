#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "MQ135.h"

#define RLOAD 22.0

LiquidCrystal_I2C lcd(0x27, 16, 2);

MQ135 gasSensor = MQ135(A0);
int sensorPin = A0;

int led1Pin = 8;
int led2Pin = 9;

float thresholdPPM = 1000.0;

void setup() {
  Serial.begin(9600);
  pinMode(sensorPin, INPUT);
  pinMode(led1Pin, OUTPUT);
  pinMode(led2Pin, OUTPUT);

  lcd.init();
  lcd.backlight();
  lcd.clear();
}

void loop() {
  int val = analogRead(sensorPin);
  Serial.print("raw = ");
  Serial.println(val);

  float ppm = gasSensor.getPPM();
  Serial.print("ppm: ");
  Serial.println(ppm);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("CO2:");
  lcd.print(ppm);
  lcd.print("PPM");
  delay(2000);

  if (ppm > thresholdPPM) {
    digitalWrite(led1Pin, HIGH);
    digitalWrite(led2Pin, HIGH);
  } else {
    digitalWrite(led1Pin, LOW);
    digitalWrite(led2Pin, LOW);
  }
}
