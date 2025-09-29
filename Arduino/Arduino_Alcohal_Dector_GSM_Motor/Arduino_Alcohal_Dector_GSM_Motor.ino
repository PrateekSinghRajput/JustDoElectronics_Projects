#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <SoftwareSerial.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);
SoftwareSerial mySerial(2, 3);

int alcoholSensor = A0;
int data = 0;

int Red = 8;
int Green = 9;
int Buzzer = 12;
int Relay = 5;

bool alertSent = false;
bool systemStartSent = false;

void setup() {
  randomSeed(analogRead(0));
  mySerial.begin(9600);
  Serial.begin(9600);

  lcd.init();
  lcd.backlight();

  pinMode(alcoholSensor, INPUT);
  pinMode(Red, OUTPUT);
  pinMode(Green, OUTPUT);
  pinMode(Buzzer, OUTPUT);
  pinMode(Relay, OUTPUT);

  lcd.print("   WELCOME TO");
  lcd.setCursor(0, 1);
  lcd.print(" JSUTDOELECTRONIC");
  delay(3000);
  lcd.clear();

  lcd.print(" Alcohol Vapor ");
  lcd.setCursor(0, 1);
  lcd.print("  Detector ");
  delay(3000);
  lcd.clear();

  digitalWrite(Red, LOW);
  digitalWrite(Green, HIGH);
  digitalWrite(Buzzer, LOW);
  digitalWrite(Relay, LOW);

  if (!systemStartSent) {
    SendSystemStartSMS();
    systemStartSent = true;
  }
}

void loop() {
  data = analogRead(alcoholSensor);

  Serial.print("Alcohol Level: ");
  Serial.println(data);

  lcd.setCursor(0, 0);
  lcd.print("Scan: ALCOHOL ON ");
  lcd.setCursor(0, 1);
  lcd.print("Value: ");
  lcd.print(data);
  delay(1000);

  if (data > 120) {
    if (!alertSent) {
      SendAlertSMS();
      alertSent = true;
    }
    Serial.println("Alcohol detected! Alarm ON");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("ALCOHOL DETECTED");
    lcd.setCursor(0, 1);
    lcd.print("SMS Sent & Alarm");

    digitalWrite(Red, HIGH);
    digitalWrite(Green, LOW);
    digitalWrite(Buzzer, HIGH);
    digitalWrite(Relay, HIGH);

    delay(2000);
  } else {
    Serial.println("Alcohol Level Normal");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Alcohol Level OK");

    digitalWrite(Red, LOW);
    digitalWrite(Green, HIGH);
    digitalWrite(Buzzer, LOW);
    digitalWrite(Relay, LOW);

    delay(2000);

    alertSent = false;
  }
  lcd.clear();
}

void SendSystemStartSMS() {
  Serial.println("Sending System Start SMS...");
  mySerial.println("AT+CMGF=1");
  delay(1000);
  mySerial.println("AT+CMGS=\"+918830584864\"\r");
  delay(1000);
  mySerial.println("Alcohol Detector System is now ON. Monitoring started.");
  delay(100);
  mySerial.println((char)26);
  delay(1000);
}

void SendAlertSMS() {
  Serial.println("Sending Alert SMS...");
  mySerial.println("AT+CMGF=1");
  delay(1000);
  mySerial.println("AT+CMGS=\"+918830584864\"\r");
  delay(1000);
  mySerial.println("Alert! Alcohol Vapor Detected. Car is slow, please move to the side of the road.");
  delay(100);
  mySerial.println((char)26);
  delay(1000);
}
