//Prateek
//www.justdoelectronics.com

#include <LiquidCrystal.h>
#include <SoftwareSerial.h>
LiquidCrystal lcd(12, 11, 10, 9, 8, 7);
SoftwareSerial SIM900(2, 3);
String textForSMS;


int P_E = 4;
const int ldrPin = A0;
const int ledPin = 13;
const int buzzerPin = 5;
const int laserPin = 6;

void setup() {

  Serial.begin(9600);
  SIM900.begin(9600);
  pinMode(ledPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(ldrPin, INPUT);
  pinMode(laserPin, OUTPUT);
  digitalWrite(laserPin, HIGH);
  pinMode(P_E, OUTPUT);

  lcd.begin(16, 2);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("  Welcome To");
  lcd.setCursor(0, 1);
  lcd.print(".............");
  delay(2000);
}

void loop() {

  int ldrStatus = analogRead(ldrPin);

  if (ldrStatus < 700) {

    digitalWrite(buzzerPin, HIGH);
    digitalWrite(ledPin, HIGH);
    Serial.println(" ALARM ACTIVATED ");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(" LASER SECURITY");
    lcd.setCursor(0, 1);
    lcd.print("ALARM ACTIVATED");
    textForSMS = "\nLaser Sensor Alert";
    sendSMS(textForSMS);
    Serial.println(textForSMS);
    Serial.println("message sent.");


    SIM900.println("ATD+ +91883058486X;");
    delay(400);

  }

  else {

    digitalWrite(buzzerPin, LOW);
    digitalWrite(ledPin, LOW);
    Serial.println("ALARM DEACTIVATED");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(" LASER SECURITY");
    lcd.setCursor(0, 1);
    lcd.print("ALARM DEACTIVATED");
  }
  Serial.println(ldrStatus);
  //delay(10);
}
void sendSMS(String message) {
  SIM900.print("AT+CMGF=1\r");
  delay(1000);
  SIM900.println("AT + CMGS = \"+91883058486X\"");
  delay(1000);
  SIM900.println(message);
  delay(1000);
  SIM900.println((char)26);
  delay(1000);
  SIM900.println();
  delay(100);
}