//Prateek
//www.justdoelectronics.com

#include <Wire.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

const int AirValue = 3620;
const int WaterValue = 1680;
const int SensorPin = 32;
int soilMoistureValue = 0;
int soilmoisturepercent = 0;
int red = 25;
int green = 14;

BlynkTimer timer;

char auth[] = "AAAAAAAAAAAAAAA";
char ssid[] = "Prateek";
char pass[] = "8830584864xx";

void setup() {
  Serial.begin(115200);
  Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);
  pinMode(red, OUTPUT);
  pinMode(green, OUTPUT);
}

void sensor() {
  soilMoistureValue = analogRead(SensorPin);
  Serial.println(soilMoistureValue);
  soilmoisturepercent = map(soilMoistureValue, AirValue, WaterValue, 0, 100);

  if (soilmoisturepercent > 100) {
    Serial.println("100 %");
    digitalWrite(red, HIGH);
    digitalWrite(green, LOW);
    WidgetLED LED(V1);
    LED.on();
  } else if (soilmoisturepercent < 50) {
    Serial.println("50 %");
    digitalWrite(green, HIGH);
    digitalWrite(red, LOW);
    WidgetLED LED(V1);
    LED.off();

  } else if (soilmoisturepercent >= 0 && soilmoisturepercent <= 100) {
    Serial.print(soilmoisturepercent);
    Serial.println("%");
    Blynk.virtualWrite(V1, soilMoistureValue);
  }
}

void loop() {
  sensor();
  Blynk.run();
  delay(200);
}