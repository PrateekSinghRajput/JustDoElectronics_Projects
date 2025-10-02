#include <HX711_ADC.h>  // https://github.com/olkal/HX711_ADC
#include <Wire.h>
#include <LiquidCrystal.h>

LiquidCrystal lcd(12, 11, 10, 9, 8, 7);
HX711_ADC LoadCell(5, 4);
const int taree = 6;

void setup() {
  pinMode(taree, INPUT_PULLUP);

  LoadCell.begin();
  LoadCell.start(1000);
  LoadCell.setCalFactor(375);

  lcd.begin(16, 2);
  lcd.setCursor(1, 0);
  lcd.print("Digital Scale");
  lcd.setCursor(0, 1);
  lcd.print(" 10KG MAX LOAD");
  delay(3000);
  lcd.clear();
}

void loop() {
  LoadCell.update();
  float weight = LoadCell.getData();

  lcd.setCursor(1, 0);
  lcd.print("Digital Scale");

  if (weight < 0) {
    weight = -weight;
    lcd.setCursor(0, 1);
    lcd.print("-");
    lcd.setCursor(8, 1);
    lcd.print("-");
  } else {
    lcd.setCursor(0, 1);
    lcd.print(" ");
    lcd.setCursor(8, 1);
    lcd.print(" ");
  }

  lcd.setCursor(1, 1);
  lcd.print(weight, 1);
  lcd.print("g ");

  float ounces = weight / 28.3495;
  lcd.setCursor(9, 1);
  lcd.print(ounces, 2);
  lcd.print("oz ");

  if (weight >= 5000) {
    lcd.setCursor(0, 0);
    lcd.print("  Over Loaded   ");
    delay(200);
  }

  if (digitalRead(taree) == LOW) {
    lcd.setCursor(0, 1);
    lcd.print("Load Reset       ");
    LoadCell.start(1000);
    lcd.setCursor(0, 1);
    lcd.print("                ");
  }
}
