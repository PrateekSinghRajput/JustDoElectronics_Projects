
//Prateek
//https://www.youtube.com/c/JustDoElectronics/videos

#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);

#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"
DFRobotDFPlayerMini myDFPlayer;
void printDetail(uint8_t type, int value);

SoftwareSerial mySoftwareSerial(2, 3);

void setup() {

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Sign Language To");
  lcd.setCursor(0, 1);
  lcd.print("Speech Convert");
  delay(4000);
  lcd.clear();

  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);

  Serial.begin(9600);
  mySoftwareSerial.begin(9600);
  Serial.begin(115200);
  Serial.println();
  Serial.println(F("Initializing DFPlayer..."));

  if (!myDFPlayer.begin(mySoftwareSerial)) {
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    while (true)
      ;
  }
  Serial.println(F("DFPlayer Mini online."));
  myDFPlayer.volume(30);
}

void loop() {

  int one = analogRead(A0);
  int two = analogRead(A1);
  int three = analogRead(A2);

  Serial.print("Sensor1 ");
  Serial.print(one);
  Serial.println("\t");

  //One
  if (one <= 150) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("I Am Hungry");
    delay(500);
    myDFPlayer.play(1);
    delay(1000);
  }

  if (one >= 300) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Help Me To");
    lcd.setCursor(0, 1);
    lcd.print("Reach Home");
    delay(500);
    myDFPlayer.play(2);
    delay(1000);

  }

  //Two
  else if (two <= 240) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Call The Police");

    delay(500);
    myDFPlayer.play(3);
    delay(1000);
  }

  //Three
  else if (three <= 215) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("I Need Water");
    lcd.setCursor(1, 1);
    lcd.print("");
    delay(500);
    myDFPlayer.play(5);
    delay(1000);

  }

  else {
    lcd.clear();
    lcd.setCursor(1, 0);
    lcd.print("NOTHING");
  }
  delay(50);
}
