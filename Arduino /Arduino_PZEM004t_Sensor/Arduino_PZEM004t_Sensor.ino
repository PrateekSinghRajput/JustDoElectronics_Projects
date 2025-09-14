//Prateek
//www.justdoelectronics.com

#include <Wire.h>
#include <PZEM004Tv30.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);
PZEM004Tv30 pzem(D7, D8);

void setup() {
  Serial.begin(115200);
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("PZEM Test");
  lcd.setCursor(0, 1);
  lcd.print("by Prateek");
  delay(500);
  lcd.clear();
}

void loop() {
  float voltage = pzem.voltage();
  if (voltage != NAN) {
    Serial.print("Voltage: ");
    Serial.print(voltage);
    Serial.println("V");
    lcd.setCursor(0, 0);
    lcd.print("V:");
    lcd.print(voltage);
  } else {
    Serial.println("Error reading voltage");
  }
  float current = pzem.current();
  if (current != NAN) {
    Serial.print("Current: ");
    Serial.print(current);
    Serial.println("A");
    lcd.setCursor(0, 1);
    lcd.print("I:");
    lcd.print(current);
  } else {
    Serial.println("Error reading current");
  }
  float power = pzem.power();
  if (current != NAN) {
    Serial.print("Power: ");
    Serial.print(power);
    Serial.println("W");
    lcd.setCursor(9, 0);
    lcd.print("P:");
    lcd.print(power);
  } else {
    Serial.println("Error reading power");
  }
  float energy = pzem.energy();
  if (current != NAN) {
    Serial.print("Energy: ");
    Serial.print(energy, 3);
    Serial.println("kWh");
  } else {
    Serial.println("Error reading energy");
  }
  float frequency = pzem.frequency();
  if (current != NAN) {
    Serial.print("Frequency: ");
    Serial.print(frequency, 1);
    Serial.println("Hz");
    lcd.setCursor(8, 1);
    lcd.print("f:");
    lcd.print(frequency);
  } else {
    Serial.println("Error reading frequency");
  }
  float pf = pzem.pf();
  if (current != NAN) {
    Serial.print("PF: ");
    Serial.println(pf);
  } else {
    Serial.println("Error reading power factor");
  }
  Serial.println();
  delay(2000);
}