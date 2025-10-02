#include <Wire.h>
#include <Adafruit_BMP085.h>
#include <LiquidCrystal.h>

Adafruit_BMP085 bmp;
LiquidCrystal lcd(12, 11, 10, 9, 8, 7);

const int ledHigh = 6;
const int ledLow = 5;
const int buzzer = 4;

const float highTempThreshold = 37.0;

void setup() {
  Serial.begin(9600);

  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085/BMP180 sensor, check wiring!");
    while (1)
      ;
  }

  lcd.begin(16, 2);
  lcd.print("Temp Monitor");
  delay(2000);
  lcd.clear();

  pinMode(ledHigh, OUTPUT);
  pinMode(ledLow, OUTPUT);
  pinMode(buzzer, OUTPUT);
}

void loop() {
  float temperature = bmp.readTemperature();

  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" C");

  lcd.setCursor(0, 0);
  lcd.print("Temp C: ");
  lcd.print(temperature, 2);
  lcd.print("   ");

  if (temperature >= highTempThreshold) {
    digitalWrite(ledHigh, HIGH);
    digitalWrite(ledLow, LOW);
    tone(buzzer, 1000);
  } else {
    digitalWrite(ledHigh, LOW);
    digitalWrite(ledLow, HIGH);
    noTone(buzzer);
  }
  delay(1000);
}
