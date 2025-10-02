#include <Wire.h>
#include <Adafruit_MLX90614.h>
#include <LiquidCrystal.h>

Adafruit_MLX90614 mlx = Adafruit_MLX90614();
LiquidCrystal lcd(12, 11, 10, 9, 8, 7);

const int ledHigh = 6;
const int ledLow = 5;
const int buzzer = 4;

const float highTempThreshold = 37.0;

void setup() {
  Serial.begin(9600);
  if (!mlx.begin()) {
    Serial.println("Error connecting to MLX90614 sensor. Check wiring!");
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
  float objectTemp = mlx.readObjectTempC();

  Serial.print("Object Temp: ");
  Serial.print(objectTemp);
  Serial.println(" C");

  lcd.setCursor(0, 0);
  lcd.print("Temp C: ");
  lcd.print(objectTemp, 2);
  lcd.print("  ");

  if (objectTemp >= highTempThreshold) {
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
