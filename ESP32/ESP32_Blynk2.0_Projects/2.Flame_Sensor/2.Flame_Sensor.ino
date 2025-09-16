//Prateek
//www.justdoelectronics.com

#include <Wire.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

#define DO_PIN 14
#define led 13
#define buzzer 12

BlynkTimer timer;

char auth[] = "AAAAAAAAAAAAAAA";
char ssid[] = "Prateek";
char pass[] = "8830584864xx";

void setup() {
  Serial.begin(9600);
  Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);
  pinMode(DO_PIN, INPUT);
  pinMode(led, OUTPUT);
  pinMode(buzzer, OUTPUT);
}

void sensor() {
  int flame_state = digitalRead(DO_PIN);

  if (flame_state == HIGH) {
    Serial.println("No flame dected => The fire is NOT detected");
    digitalWrite(led, HIGH);
    digitalWrite(buzzer, LOW);
    WidgetLED LED(V2);
    LED.off();
  }

  else {
    Serial.println("Flame dected => The fire is detected");
    digitalWrite(led, LOW);
    digitalWrite(buzzer, HIGH);
    WidgetLED LED(V2);
    LED.on();
  }
}

void loop() {
  sensor();
  Blynk.run();
  delay(200);
}