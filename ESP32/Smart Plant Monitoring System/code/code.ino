//Prateek
//www.justdoelectronics.com

#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <DHT.h>

#define TH 14
#define Rain 35
#define Soil 34
#define redled 12
#define yellowled 13
#define buzzer 27
#define relay 25

LiquidCrystal_I2C lcd(0x27, 16, 2);

DHT dht(TH, DHT11);
BlynkTimer timer;

char auth[] = "94NxhMmZjXcITURaB1DGgI6_AsWk0Bj";
char ssid[] = "justdoelectronics";
char pass[] = "123456789@#";

void setup() {

  Serial.begin(115200);
  Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);
  dht.begin();
  lcd.init();
  lcd.backlight();
  pinMode(Rain, INPUT);
  pinMode(Soil, INPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(redled, OUTPUT);
  pinMode(yellowled, OUTPUT);
  pinMode(relay, OUTPUT);

  lcd.setCursor(0, 0);
  lcd.print("System");
  lcd.setCursor(4, 1);
  lcd.print("Loading..");
  delay(4000);
  lcd.clear();
}

void DHT11sensor() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  Blynk.virtualWrite(V0, t);
  Blynk.virtualWrite(V1, h);

  lcd.setCursor(0, 0);
  lcd.print("T:");
  lcd.print(t);

  lcd.setCursor(8, 0);
  lcd.print("H:");
  lcd.print(h);
}

//Get the rain sensor values
void rainSensor() {
  int Rvalue = analogRead(Rain);
  Rvalue = map(Rvalue, 0, 4095, 0, 100);
  Rvalue = (Rvalue - 100) * -1;
  Blynk.virtualWrite(V4, Rvalue);

  lcd.setCursor(0, 1);
  lcd.print("R:");
  lcd.print(Rvalue);
  lcd.print(" ");
  Serial.println(Rvalue);

  if (Rvalue >= 20) {
    digitalWrite(redled, HIGH);
    digitalWrite(buzzer, HIGH);
    WidgetLED LED1(V3);
    LED1.on();
  } else {
    digitalWrite(redled, LOW);
    digitalWrite(buzzer, LOW);
    WidgetLED LED1(V3);
    LED1.off();
  }
}

void soilSensor() {
  int Rvalue1 = analogRead(Soil);
  Rvalue1 = map(Rvalue1, 0, 4095, 0, 100);
  Rvalue1 = (Rvalue1 - 100) * -1;
  Blynk.virtualWrite(V2, Rvalue1);

  lcd.setCursor(8, 1);
  lcd.print("S:");
  lcd.print(Rvalue1);
  lcd.print(" ");
  Serial.println(Rvalue1);

  if (Rvalue1 >= 40) {
    digitalWrite(yellowled, HIGH);
    digitalWrite(buzzer, HIGH);
    digitalWrite(relay, LOW);

  } else {
    digitalWrite(yellowled, LOW);
    digitalWrite(buzzer, LOW);
    digitalWrite(relay, HIGH);
  }
}

void loop() {
  DHT11sensor();
  rainSensor();
  soilSensor();
  Blynk.run();
  delay(500);
}