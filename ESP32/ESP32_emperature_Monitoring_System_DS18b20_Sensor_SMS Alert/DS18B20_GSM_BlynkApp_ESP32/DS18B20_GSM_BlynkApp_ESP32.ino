//Prateek
//www.justdoelectronics.com

#define BLYNK_PRINT Serial
#include <WiFi.h>
#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <BlynkSimpleEsp32.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);

#include <SoftwareSerial.h>
SoftwareSerial mySerial(16, 17);

#define BLYNK_PRINT Serial
#define ONE_WIRE_BUS 14

#define fanPin 13
int buzzer = 12;
int dutyCycle = 0;

float temp = 0;
int threshold = 40;

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

WidgetLED FAN(V0);

char auth[] = "B3adyvbdhQE9MoFfnQLOI5n96-BaaY4v";
char ssid[] = "Prateek";
char pass[] = "12345678prateek";

void setup() {
  mySerial.begin(9600);
  Serial.begin(115200);
  sensors.begin();
  pinMode(buzzer, OUTPUT);
  pinMode(fanPin, OUTPUT);
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("  Temperature ");
  lcd.setCursor(0, 1);
  lcd.print("Monitoring System");
  delay(4000);
  lcd.clear();

  Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);
}

BLYNK_WRITE(V1) {
  threshold = param.asInt();
  Serial.print(" The Threshhold thresholdue is: ");
  Serial.println(threshold);
  Serial.println();
}

void loop() {
  Blynk.run();
  sensors.requestTemperatures();
  temp = sensors.getTempCByIndex(0);

  Serial.print("Temperature: ");
  Serial.print(temp);
  Serial.println(" C");

  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.print(temp);
  lcd.print(" C");

  Blynk.virtualWrite(V2, temp);

  if (temp >= threshold) {
    FAN.on();
    SendMessage();
    lcd.setCursor(0, 1);
    lcd.print("Fan : ");
    lcd.print(".ON");
   digitalWrite(fanPin, LOW);
    digitalWrite(buzzer, HIGH);

  } else if (temp < threshold) {
    FAN.off();
    lcd.setCursor(0, 1);
    lcd.print("Fan : ");
    lcd.print("OFF");
      digitalWrite(fanPin, HIGH);
      digitalWrite(buzzer, LOW);
  }
}

void SendMessage() {
  mySerial.println("AT+CMGF=1");
  delay(1000);
  mySerial.println("AT+CMGS=\"+9188305848xx\"\r");
  delay(1000);
  mySerial.println("Temperature is High");
  delay(100);
  mySerial.println((char)26);
  delay(1000);
}
