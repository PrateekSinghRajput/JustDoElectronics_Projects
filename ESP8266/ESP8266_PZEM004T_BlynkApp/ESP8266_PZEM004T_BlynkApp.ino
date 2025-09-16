

#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <PZEM004Tv30.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
PZEM004Tv30 pzem(&Serial);

#define BLYNK_TEMPLATE_ID "TMPL3I736Szm1"
#define BLYNK_TEMPLATE_NAME "Smart Energy Meter"
#define BLYNK_AUTH_TOKEN "YqP9LI3zowGBCLYDmosMDVsfu-WMziph"

char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "prateeksingh";
char pass[] = "kumar@12345";

float voltage = 0;
float current = 0;
float power = 0;
float energy = 0;
float frequency = 0;
float pf = 0;
unsigned long lastMillis = 0;
LiquidCrystal_I2C lcd(0x27, 20, 4);

void setup() {
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("    Energy meter    ");
  delay(1000);
  lcd.setCursor(0, 1);
  lcd.print("   Monitor by IOT   ");
  delay(1000);
  lcd.setCursor(0, 2);
  lcd.print("     Designed by   ");
  delay(1000);
  lcd.setCursor(0, 3);
  lcd.print(" JustDoElectronics");
  delay(3000);
  lcd.clear();
  lcd.print("Device connecting...");
  delay(2000);
  lcd.setCursor(0, 2);
  lcd.print(".......please  wait");

  Serial.begin(9600);
  Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);
  lcd.clear();
}
void loop() {

  Blynk.run();

  float voltage = pzem.voltage();
  if (!isnan(voltage)) {
    Serial.print("Voltage: ");
    Serial.print(voltage, 1);
    Serial.println("V");
    lcd.setCursor(0, 0);
    lcd.print("Voltage:      Volts");
    lcd.setCursor(8, 0);
    lcd.print(voltage, 1);
  } else {
    Serial.println("Error reading voltage");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Voltage = 0.00Volts ");
    lcd.setCursor(0, 1);
    lcd.print("Current = 0 Amps  ");
    lcd.setCursor(0, 3);
    lcd.print("  No Power  Supply  ");
  }

  float current = pzem.current();
  if (!isnan(current)) {
    Serial.print("Current: ");
    Serial.print(current, 3);
    Serial.println("A");
    lcd.setCursor(0, 1);
    lcd.print("Current:      Amps");
    lcd.setCursor(8, 1);
    lcd.print(current, 3);
  } else {
    Serial.println("Error reading current");
  }

  float power = pzem.power();
  if (!isnan(power)) {
    Serial.print("Power: ");
    Serial.print(power, 0);
    Serial.println("W");
    lcd.setCursor(0, 2);
    lcd.print("     Watts");
    lcd.setCursor(0, 2);
    lcd.print(power, 0);
  } else {
    Serial.println("Error reading power");
  }

  float energy = pzem.energy();
  if (!isnan(energy)) {
    Serial.print("Energy: ");
    Serial.print(energy, 3);
    Serial.println("kWh");
    lcd.setCursor(12, 2);
    lcd.print("     Kwh");
    lcd.setCursor(12, 2);
    lcd.print(energy, 3);
  } else {
    Serial.println("Error reading energy");
  }

  float frequency = pzem.frequency();
  if (!isnan(frequency)) {
    Serial.print("Frequency: ");
    Serial.print(frequency, 1);
    Serial.println("Hz");
    lcd.setCursor(0, 3);
    lcd.print("Freq:    Hz");
    lcd.setCursor(5, 3);
    lcd.print(frequency, 1);
  } else {
    Serial.println("Error reading frequency");
  }

  float pf = pzem.pf();
  if (!isnan(pf)) {
    Serial.print("PF: ");
    Serial.println(pf);
    lcd.setCursor(12, 3);
    lcd.print("PF: ");
    lcd.setCursor(15, 3);
    lcd.print(pf);
  } else {
    Serial.println("Error reading power factor");
  }

  Serial.println();
  delay(2000);


  delay(1000);

  if (millis() - lastMillis > 5000) {
    lastMillis = millis();

    Blynk.virtualWrite(V0, voltage);
    Blynk.virtualWrite(V1, current);
    Blynk.virtualWrite(V3, power);
    Blynk.virtualWrite(V4, energy);
    Blynk.virtualWrite(V2, frequency);
    Blynk.virtualWrite(V6, pf);
  }
}
