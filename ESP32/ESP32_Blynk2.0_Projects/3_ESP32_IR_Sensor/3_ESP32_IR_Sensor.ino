#include <Wire.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

#define YELLOW_SENSOR_PIN 15
#define led 12
#define buzzer 13

BlynkTimer timer;

char auth[] = "xxxxxxxxxxxx";
char ssid[] = "Prateek";
char pass[] = "12345@#12345";

void setup() {
  Serial.begin(9600);
  Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);
  pinMode(YELLOW_SENSOR_PIN, INPUT);
  pinMode(led, OUTPUT);
  pinMode(buzzer, OUTPUT);
}

void sensor() {
  int yellow_state = digitalRead(YELLOW_SENSOR_PIN);

  if (yellow_state == HIGH) {
    Serial.println("Yellow sensor inactive");
    digitalWrite(led, HIGH);
    digitalWrite(buzzer, LOW);
    WidgetLED LED(V2);
    LED.off();
  } else {
    Serial.println("Yellow sensor active");
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
