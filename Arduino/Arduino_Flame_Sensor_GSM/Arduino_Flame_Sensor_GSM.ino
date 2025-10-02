#include <LiquidCrystal_I2C.h>
#include <Wire.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

int flameSensor = 7;
int Red = 4;
int Green = 6;
int Buzzer = 5;
int Relay = 3;

bool alertSent = false;
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

  lcd.print(" Flame Detection ");
  lcd.setCursor(0, 1);
  lcd.print("    System ");
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
  Serial.print("Flame Sensor State: ");
  Serial.println(flameState);

  if (flameState == LOW) {
    if (!alertSent) {
      SendAlertSMS();
      alertSent = true;
    }
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("FLAME DETECTED   ");
    lcd.setCursor(0, 1);
    lcd.print("SMS Sent & Alarm ");

    digitalWrite(Red, HIGH);
    digitalWrite(Green, LOW);
    digitalWrite(Buzzer, HIGH);
    digitalWrite(Relay, HIGH);

    delay(2000);
  } else {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("No Flame Detected");

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
  Serial.println("AT+CMGF=1");
  delay(1000);
  Serial.println("AT+CMGS=\"+9188305848xx\"\r");
  delay(1000);
  Serial.println("Flame Detector System is now ON. Monitoring started.");
  delay(100);
  Serial.write((char)26);
  delay(1000);
}

void SendAlertSMS() {
  Serial.println("AT+CMGF=1");
  delay(1000);
  Serial.println("AT+CMGS=\"+9188305848xx\"\r");
  delay(1000);
  Serial.println("Alert! Flame Detected. Please slow down and stop your vehicle safely!");
  delay(100);
  Serial.write((char)26);
  delay(1000);
}
