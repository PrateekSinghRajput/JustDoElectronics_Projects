
//Prateek
//wwww.prateeks.in
//https://www.youtube.com/c/JustDoElectronics/videos

#include <LiquidCrystal.h>
LiquidCrystal lcd(12, 11, 10, 9, 8, 7);//RS,EN,D4,D5,D6,D7

#include "SoftwareSerial.h"
#define blue 2
#define green 3
#define red 4
unsigned int f;


void setup()
{
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("Sign Language to");
  lcd.setCursor(0, 1);
  lcd.print("Speech Convert");
  delay(6000);
  lcd.clear();
  pinMode(blue, OUTPUT);
  pinMode(green, OUTPUT);
  pinMode(red, OUTPUT);
  Serial.begin(9600);
  Serial.println();



}

void loop()
{
  f = analogRead(0);
  Serial.println(f);
  if (f > 720)                       //No Bend; Blue LED Glows
  {
    digitalWrite(blue, HIGH);
    digitalWrite(green, LOW);
    digitalWrite(red, LOW);
    Serial.println("Please Give Me Water");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Cmd 1");
    lcd.setCursor(0, 1);
    lcd.print(" Give Me Water");
    delay(1000);
  }
  else if ((f < 320) && (f > 280))   //Small Bend; Green LED Glows
  {
    digitalWrite(green, HIGH);
    digitalWrite(blue, LOW);
    digitalWrite(red, LOW);
    Serial.println("Please Give Me Food");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Cmd 2");
    lcd.setCursor(0, 1);
    lcd.print("Give Me Food");
    delay(1000);
  }
  else if ((f < 200) && (f > 100))   //Small Bend; Green LED Glows
  {
    digitalWrite(green, HIGH);
    digitalWrite(blue, LOW);
    digitalWrite(red, LOW);
    Serial.println("Give Me Water");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Cmd 3");
    lcd.setCursor(0, 1);
    lcd.print("Give Me Water");
    delay(1000);
  }
  else                              // Larger Bend; RED LED Glows
  {
    if (f > 100)
      digitalWrite(red, HIGH);
    digitalWrite(green, LOW);
    digitalWrite(blue, LOW);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("No Sign Language");
  }
  delay(50);
}
