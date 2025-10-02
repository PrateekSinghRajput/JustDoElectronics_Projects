#include <LiquidCrystal_I2C.h>
#include <Wire.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

int flameSensor = 7;
int smokeSensor = A0;
int smokeThreshold = 400;

int Red = 4;
int Green = 6;
int Buzzer = 5;
int Relay = 3;

bool flameAlertSent = false;
bool smokeAlertSent = false;
bool systemStartSent = false;

void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();

  pinMode(flameSensor, INPUT);
  pinMode(Red, OUTPUT);
  pinMode(Green, OUTPUT);
  pinMode(Buzzer, OUTPUT);
  pinMode(Relay, OUTPUT);

  lcd.print("   WELCOME TO");
  lcd.setCursor(0, 1);
  lcd.print(" JSUTDOELECTRONIC");
  delay(3000);
  lcd.clear();

  lcd.print(" Flame & Smoke ");
  lcd.setCursor(0, 1);
  lcd.print(" Detection Sys ");
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
  int flameState = digitalRead(flameSensor);
  int smokeLevel = analogRead(smokeSensor);

  bool flameDetected = (flameState == LOW);
  bool smokeDetected = (smokeLevel > smokeThreshold);

  Serial.print("Flame: ");
  Serial.print(flameState);
  Serial.print(", Smoke Level: ");
  Serial.println(smokeLevel);

  if (flameDetected) {
    if (!flameAlertSent) {
      SendFlameAlertSMS();
      flameAlertSent = true;
      smokeAlertSent = false;  // Reset to allow smoke alerts later
    }
    ActivateAlarm();
    lcdStatus("FLAME DETECTED", "SMS Sent & Alarm");
  } else if (smokeDetected) {
    if (!smokeAlertSent) {
      SendSmokeAlertSMS();
      smokeAlertSent = true;
      flameAlertSent = false;  // Reset for flame alerts later
    }
    ActivateAlarm();
    lcdStatus("SMOKE DETECTED", "SMS Sent & Alarm");
  } else {
    flameAlertSent = false;
    smokeAlertSent = false;
    DeactivateAlarm();
    lcdStatus("ENV SAFE", "Monitoring...");
  }

  delay(2000);
  lcd.clear();
}

void ActivateAlarm() {
  digitalWrite(Red, HIGH);
  digitalWrite(Green, LOW);
  digitalWrite(Buzzer, HIGH);
  digitalWrite(Relay, HIGH);
}

void DeactivateAlarm() {
  digitalWrite(Red, LOW);
  digitalWrite(Green, HIGH);
  digitalWrite(Buzzer, LOW);
  digitalWrite(Relay, LOW);
}

void lcdStatus(const char* line1, const char* line2) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(line1);
  lcd.setCursor(0, 1);
  lcd.print(line2);
}

void SendSystemStartSMS() {
  Serial.println("AT+CMGF=1");
  delay(1000);
  Serial.println("AT+CMGS=\"+9188305848xx\"\r");
  delay(1000);
  Serial.println("Flame & Smoke Detector System ON. Monitoring started.");
  delay(100);
  Serial.write((char)26);
  delay(1000);
}

void SendFlameAlertSMS() {
  Serial.println("AT+CMGF=1");
  delay(1000);
  Serial.println("AT+CMGS=\"+9188305848xx\"\r");
  delay(1000);
  Serial.println("Fire detected! Evacuate immediately!");
  delay(100);
  Serial.write((char)26);
  delay(1000);
}

void SendSmokeAlertSMS() {
  Serial.println("AT+CMGF=1");
  delay(1000);
  Serial.println("AT+CMGS=\"+9188305848xx\"\r");
  delay(1000);
  Serial.println("Smoke detected! Evacuate immediately!");
  delay(100);
  Serial.write((char)26);
  delay(1000);
}
