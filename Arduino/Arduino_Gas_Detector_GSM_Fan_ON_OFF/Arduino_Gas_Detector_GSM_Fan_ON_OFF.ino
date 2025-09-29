#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <SoftwareSerial.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);
SoftwareSerial mySerial(2, 3);

int gasSensor = A0;
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

  pinMode(gasSensor, INPUT);
  pinMode(Red, OUTPUT);
  pinMode(Green, OUTPUT);
  pinMode(Buzzer, OUTPUT);
  pinMode(Relay, OUTPUT);

  lcd.print("   WELCOME TO");
  lcd.setCursor(0, 1);
  lcd.print(" JSUTDOELECTRONIC");
  delay(3000);
  lcd.clear();

  lcd.print("  GAS LEAKAGE  ");
  lcd.setCursor(0, 1);
  lcd.print("   DETECTOR    ");
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
  data = analogRead(gasSensor);

  Serial.print("Gas Sensor Value: ");
  Serial.println(data);

  lcd.setCursor(0, 0);
  lcd.print("Scanning GAS...");
  lcd.setCursor(0, 1);
  lcd.print("Value: ");
  lcd.print(data);
  delay(1000);

  if (data > 120) {
    if (!alertSent) {
      SendAlertSMS();
      alertSent = true;
    }

    Serial.println("Gas Leakage Detected! Alarm ON");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(" GAS LEAKAGE!! ");
    lcd.setCursor(0, 1);
    lcd.print("SMS Sent & Alarm");

    digitalWrite(Red, HIGH);
    digitalWrite(Green, LOW);
    digitalWrite(Buzzer, HIGH);
    digitalWrite(Relay, HIGH);

    delay(2000);
  } else {
    Serial.println("Gas Level Normal");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(" Gas Level OK  ");

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
  mySerial.println("Gas Leakage Detector is now ON. Monitoring started.");
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
  mySerial.println("!! WARNING !! Gas Leakage Detected. Please check immediately and ensure safety.");
  delay(100);
  mySerial.println((char)26);
  delay(1000);
}
