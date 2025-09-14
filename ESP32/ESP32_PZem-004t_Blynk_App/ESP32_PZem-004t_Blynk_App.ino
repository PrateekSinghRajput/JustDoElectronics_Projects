#include <PZEM004Tv30.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);
PZEM004Tv30 pzem(Serial2, 16, 17);

#define relay1 18

BlynkTimer timer;

char auth[] = "-Wm1OT7IFsfdFB6Hp4nlots5WsGCfC";
char ssid[] = "Prateek";
char pass[] = "12345@#12345";


BLYNK_WRITE(V6) {
  bool value1 = param.asInt();

  if (value1 == 1) {
    digitalWrite(relay1, LOW);
  } else {
    digitalWrite(relay1, HIGH);
  }
}


void setup() {
  Serial.begin(115200);
  Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);
  pinMode(relay1, OUTPUT);
  digitalWrite(relay1, HIGH);
  lcd.init();
  lcd.backlight();
}

void loop() {
  current();
  Blynk.run();
  delay(200);
}

void current() {
  float voltage = pzem.voltage();
  if (voltage != NAN) {
    Serial.print("Voltage: ");
    Serial.print(voltage);
    Serial.println("V");
    lcd.setCursor(0, 0);
    lcd.print("V:");
    lcd.print(voltage);
    Blynk.virtualWrite(V0, voltage);
  } else {
    Serial.println("Error reading voltage");
  }
  float current = pzem.current();
  if (current != NAN) {
    Serial.print("Current: ");
    Serial.print(current);
    Serial.println("A");
    lcd.setCursor(0, 1);
    lcd.print("A:");
    lcd.print(current);
    Blynk.virtualWrite(V1, current);

  } else {
    Serial.println("Error reading current");
  }
  float power = pzem.power();
  if (current != NAN) {
    Serial.print("Power: ");
    Serial.print(power);
    Serial.println("W");
    lcd.setCursor(8, 1);
    lcd.print("W: ");
    lcd.print(power);
    Blynk.virtualWrite(V2, power);
  } else {
    Serial.println("Error reading power");
  }
  float energy = pzem.energy();
  if (current != NAN) {
    Serial.print("Energy: ");
    Serial.print(energy, 3);
    Serial.println("kWh");
    Blynk.virtualWrite(V4, energy, 3);
  } else {
    Serial.println("Error reading energy");
  }
  float frequency = pzem.frequency();
  if (current != NAN) {
    Serial.print("Frequency: ");
    Serial.print(frequency, 1);
    Serial.println("Hz");
    lcd.setCursor(8, 0);
    lcd.print("F: ");
    lcd.print(frequency);
    Blynk.virtualWrite(V3, frequency);
  } else {
    Serial.println("Error reading frequency");
  }
  float pf = pzem.pf();
  if (current != NAN) {
    Serial.print("PF: ");
    Serial.println(pf);
    Blynk.virtualWrite(V5, pf);
  } else {
    Serial.println("Error reading power factor");
  }
  Serial.println();
  delay(2000);
}