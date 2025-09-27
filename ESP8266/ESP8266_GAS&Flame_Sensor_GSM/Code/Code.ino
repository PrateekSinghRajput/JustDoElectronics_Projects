#define BLYNK_PRINT Serial

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define SDA_PIN D2
#define SCL_PIN D1

LiquidCrystal_I2C lcd(0x27, 16, 2);

const int flameSensorPin = D3;
const int mqSensorPin = A0;
const int buzzerPin = D5;

int flameState;
int gasValue;
const int GAS_THRESHOLD = 400;

bool fireAlertSent = false;
bool gasAlertSent = false;

// Replace with your Blynk auth token
char auth[] = "tSmQNm3g66K62wTjaEQ-ADJgfy74bqHt";
char ssid[] = "Prateek";
char pass[] = "justdoelectronics@#12345";

void setup() {
  Wire.begin(SDA_PIN, SCL_PIN);
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("  WELCOME TO   ");
  lcd.setCursor(0, 1);
  lcd.print(" FIRE DETECTOR ");
  delay(2000);
  lcd.clear();

  pinMode(flameSensorPin, INPUT);
  pinMode(buzzerPin, OUTPUT);

  Serial.begin(9600);
  delay(1000);

  Blynk.begin(auth, ssid, pass , "blynk.cloud", 80);
}

void sendSMS(const char* message) {
  Serial.println("AT+CMGF=1");     
  delay(500);
  Serial.println("AT+CMGS=\"+91xxxxxxxxx\"");  
  delay(500);
  Serial.print(message);
  delay(200);
  Serial.write(26);                     
  delay(2000);
}

void fireBuzzerPattern() {
  for (int i = 0; i < 3; i++) {
    digitalWrite(buzzerPin, HIGH);
    delay(200);
    digitalWrite(buzzerPin, LOW);
    delay(200);
  }
}

void gasBuzzerPattern() {
  for (int i = 0; i < 2; i++) {
    digitalWrite(buzzerPin, HIGH);
    delay(700);
    digitalWrite(buzzerPin, LOW);
    delay(700);
  }
}

void loop() {
  Blynk.run();

  flameState = digitalRead(flameSensorPin);
  gasValue = analogRead(mqSensorPin);

  Serial.print("Gas Value: ");
  Serial.println(gasValue);

  Blynk.virtualWrite(V0, gasValue);

  if (flameState == HIGH && gasValue < GAS_THRESHOLD) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("   Safe Zone   ");
    lcd.setCursor(0, 1);
    lcd.print(" Flm/Gas Normal");

    digitalWrite(buzzerPin, LOW);

    Blynk.virtualWrite(V1, LOW);

    fireAlertSent = false;
    gasAlertSent = false;

  } else if (flameState == LOW) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Fire Detected!");
    lcd.setCursor(0, 1);
    lcd.print("Alert!         ");

    if (!fireAlertSent) {
      sendSMS("Fire Detected ! Please check immediately.");
      fireAlertSent = true;
      gasAlertSent = false;
    }

    fireBuzzerPattern();

    Blynk.virtualWrite(V1, HIGH);

  } else if (gasValue >= GAS_THRESHOLD && !fireAlertSent) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(" GAS Detected! ");
    lcd.setCursor(0, 1);
    lcd.print("Be Alert!      ");

    if (!gasAlertSent) {
      sendSMS("Gas Pipe Leakage Detect ! Please check immediately.");
      gasAlertSent = true;
    }

    gasBuzzerPattern();

    Blynk.virtualWrite(V1, LOW);

  } else {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(" System Error  ");
    lcd.setCursor(0, 1);
    lcd.print(" Check Sensors ");

    digitalWrite(buzzerPin, LOW);

    fireAlertSent = false;
    gasAlertSent = false;

    Blynk.virtualWrite(V1, LOW);
  }
  delay(250);
}
