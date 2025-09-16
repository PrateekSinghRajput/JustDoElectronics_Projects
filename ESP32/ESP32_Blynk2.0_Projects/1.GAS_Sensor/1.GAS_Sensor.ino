//Prateek
//www.justdoelectronics.com

#include <Wire.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

#define sensor 32
#define buzzer 14
#define led 27

BlynkTimer timer;

char auth[] = "AAAAAAAAAAAAAAA";
char ssid[] = "Prateek";
char pass[] = "8830584864xx";


void setup() {

  Serial.begin(115200);
  Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);
  pinMode(buzzer, OUTPUT);
  pinMode(led, OUTPUT);
}

void GASLevel() {
  int value = analogRead(sensor);
  value = map(value, 0, 4095, 0, 100);
  Serial.println(value);
  Blynk.virtualWrite(V0, value);

  if (value >= 50) {
    digitalWrite(buzzer, HIGH);
    digitalWrite(led, LOW);
    WidgetLED LED(V1);
    LED.on();

  } else {
    digitalWrite(buzzer, LOW);
    digitalWrite(led, HIGH);
    WidgetLED LED(V1);
    LED.off();
  }
}

void loop() {
  GASLevel();
  Blynk.run();
  delay(200);
}
