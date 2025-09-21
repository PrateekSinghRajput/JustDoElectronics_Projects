#include <Wire.h>
#include <LiquidCrystal.h>

LiquidCrystal lcd(12, 11, 10, 9, 8, 7);

const int moisturePin = A0;
const int greenLED = 6;
const int redLED = 5;
const int relayPin = 4;

const int map_low = 1023;
const int map_high = 280;

int moistureValue;
int moisturePercentage;

void setup() {
  pinMode(moisturePin, INPUT_PULLUP);
  pinMode(greenLED, OUTPUT);
  pinMode(redLED, OUTPUT);
  pinMode(relayPin, OUTPUT);

  digitalWrite(relayPin, LOW);

  Serial.begin(9600);

  lcd.begin(16, 2);
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("Welcome To");
  lcd.setCursor(0, 1);
  lcd.print("JustDoElectronic");
  delay(2000);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Irrigation Systm");
  lcd.setCursor(0, 1);
  lcd.print("Pump=     M=   ");
}

void loop() {
  moistureValue = analogRead(moisturePin);
  Serial.println(moistureValue);

  moisturePercentage = map(moistureValue, map_low, map_high, 0, 100);

  if (moisturePercentage > 100) {
    moisturePercentage = 100;
  } else if (moisturePercentage < 0) {
    moisturePercentage = 0;
  }

  lcd.setCursor(12, 1);
  lcd.print("    ");
  lcd.setCursor(12, 1);
  lcd.print(moisturePercentage);
  lcd.print("%");

  if (moistureValue >= 780) {
    digitalWrite(greenLED, HIGH);
    digitalWrite(redLED, LOW);
    digitalWrite(relayPin, HIGH);

    lcd.setCursor(5, 1);
    lcd.print("ON ");
  } else if (moistureValue <= 770) {
    digitalWrite(greenLED, LOW);
    digitalWrite(redLED, HIGH);
    digitalWrite(relayPin, LOW);

    lcd.setCursor(5, 1);
    lcd.print("OFF");
  }

  delay(200);
}
