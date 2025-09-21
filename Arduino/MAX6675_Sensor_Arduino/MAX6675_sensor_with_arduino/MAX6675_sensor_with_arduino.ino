#include <LiquidCrystal_I2C.h>
#include "max6675.h"
#define red 12
#define buzzer 11
#define green A2

#define relay 10

LiquidCrystal_I2C lcd(0x27, 16, 2);

int thermoDO = 4;
int thermoCS = 5;
int thermoCLK = 6;

MAX6675 thermocouple(thermoCLK, thermoCS, thermoDO);

void setup() {
  Serial.begin(9600);
  pinMode(red, OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(green, OUTPUT);
   pinMode(relay, OUTPUT);
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("  Welcome To");
  lcd.setCursor(0, 1);
  lcd.print("JustDoElectronic");
  lcd.clear();
  Serial.println("MAX6675 test");
  delay(500);
}

void loop() {

  Serial.print("Temperature C = ");
  Serial.println(thermocouple.readCelsius());
  Serial.print("Temperature F = ");
  Serial.println(thermocouple.readFahrenheit());

  lcd.setCursor(0, 0);
  lcd.print("Temp C ");
  lcd.setCursor(9, 0);
  lcd.print(thermocouple.readCelsius());
  lcd.setCursor(0, 1);
  lcd.print("Temp F ");
  lcd.setCursor(9, 1);
  lcd.print(thermocouple.readFahrenheit());

  if (thermocouple.readCelsius() >= 35) {
    digitalWrite(red, HIGH);
    digitalWrite(buzzer, HIGH);
    digitalWrite(relay, HIGH);
    digitalWrite(green, LOW);
  } else {
    digitalWrite(red, LOW);
    digitalWrite(buzzer, LOW);
    digitalWrite(relay, LOW);
    digitalWrite(green, HIGH);
  }

  delay(1000);
}