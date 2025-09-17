
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);
int measurePin = A0;
int ledPower = 7;
#define relay 4
int samplingTime = 280;
int deltaTime = 40;
int sleepTime = 9680;

float voMeasured = 0;
float calcVoltage = 0;
float dustDensity = 0;

void setup() {
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("  Welcome To");
  lcd.setCursor(0, 1);
  lcd.print("JustDoElectronic");
  Serial.begin(9600);
  pinMode(ledPower, OUTPUT);
  pinMode(relay, OUTPUT);
}

void loop() {
  digitalWrite(ledPower, LOW);
  delayMicroseconds(samplingTime);

  voMeasured = analogRead(measurePin);

  delayMicroseconds(deltaTime);
  digitalWrite(ledPower, HIGH);
  delayMicroseconds(sleepTime);
  calcVoltage = voMeasured * (5.0 / 1024.0);
  dustDensity = 170 * calcVoltage - 0.1;

  Serial.print("Dust Density: ");
  Serial.print(dustDensity);
  Serial.println(" ug/m3");
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Dust Density");
  lcd.setCursor(0, 1);
  lcd.print(dustDensity);
  lcd.print(" ug/m3");

  if (dustDensity < 250) {
    digitalWrite(relay, HIGH);
  } else {
    digitalWrite(relay, LOW);
  }

  delay(1000);
}