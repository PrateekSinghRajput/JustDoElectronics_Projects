#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ZMPT101B.h>
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

#define SENSITIVITY 500.0f
#define RELAY_PIN 14


ZMPT101B voltageSensor(34, 50.0);


char auth[] = "svr-hSXJajBDcey6VtZCNmdBaUyKir";
char ssid[] = "Prateek";
char pass[] = "ju12345";


#define VIRTUAL_PIN V0
#define LED_UNDER_VOLTAGE V2
#define LED_OVER_VOLTAGE V1

void setup() {

  Serial.begin(115200);

  Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);
  voltageSensor.setSensitivity(SENSITIVITY);

  lcd.init();
  lcd.backlight();

  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);

  lcd.setCursor(0, 0);
  lcd.print("Voltage Monitor");
  delay(2000);
  lcd.clear();
}

void loop() {

  Blynk.run();

  float voltage = voltageSensor.getRmsVoltage();
  Serial.println(voltage);
  Blynk.virtualWrite(VIRTUAL_PIN, voltage);

  delay(500);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Voltage: ");
  lcd.print(voltage);
  lcd.print("V");


  if (voltage < 180.0) {
    lcd.setCursor(0, 1);
    lcd.print("Under Voltage ");
    digitalWrite(RELAY_PIN, LOW);
    Blynk.notify("Under Voltage detected!");
    Blynk.virtualWrite(LED_UNDER_VOLTAGE, 255);
    Blynk.virtualWrite(LED_OVER_VOLTAGE, 0);
  } else if (voltage > 242.0) {
    lcd.setCursor(0, 1);
    lcd.print("Over Voltage  ");
    digitalWrite(RELAY_PIN, LOW);
    Blynk.notify("Over Voltage detected!");
    Blynk.virtualWrite(LED_UNDER_VOLTAGE, 0);
    Blynk.virtualWrite(LED_OVER_VOLTAGE, 255);
  } else {
    lcd.setCursor(0, 1);
    lcd.print("Normal         ");
    digitalWrite(RELAY_PIN, HIGH);
    Blynk.virtualWrite(LED_UNDER_VOLTAGE, 0);
    Blynk.virtualWrite(LED_OVER_VOLTAGE, 0);
  }
}
