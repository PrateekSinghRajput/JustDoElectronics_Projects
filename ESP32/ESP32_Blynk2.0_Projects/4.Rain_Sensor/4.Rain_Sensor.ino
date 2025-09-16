//Prateek
//www.justdoelectronics.com

#include <Wire.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

#define buzzer 13
#define DO_PIN 19

BlynkTimer timer;

char auth[] = "AAAAAAAAAAAAAAA";
char ssid[] = "Prateek";
char pass[] = "8830584864xx";

void setup() {
  Serial.begin(9600);
  Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);
  pinMode(buzzer, OUTPUT);
  pinMode(DO_PIN, INPUT);
}

void sensor() {
  int rain_state = digitalRead(DO_PIN);

  if (rain_state == HIGH) {
    Serial.println("The rain is NOT detected");
    digitalWrite(buzzer, LOW);
    WidgetLED LED(V3);
    LED.off();
  } else {
    Serial.println("The rain is detected");
    digitalWrite(buzzer, HIGH);
    WidgetLED LED(V3);
    LED.on();
  }
}

void loop() {
  sensor();
  Blynk.run();
  delay(200);
}