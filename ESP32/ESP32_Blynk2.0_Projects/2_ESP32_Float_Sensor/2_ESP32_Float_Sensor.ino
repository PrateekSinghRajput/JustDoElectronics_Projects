//Prateek
//www.justdoelectronics.com

#include <Wire.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

#define FLOAT_SENSOR_PIN 14
#define RED_LED_PIN 13
#define GREEN_LED_PIN 12

BlynkTimer timer;

char auth[] = "xxxxxxxxxxxx";
char ssid[] = "Prateek";
char pass[] = "12345@#12345";

void setup() {
  Serial.begin(9600);
  Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);
  pinMode(FLOAT_SENSOR_PIN, INPUT);
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);
}

void sensor() {
  int water_state = digitalRead(FLOAT_SENSOR_PIN);

  WidgetLED LED(V2);

  if (water_state == HIGH) {
    Serial.println("Water present => Tank is NOT empty");
    digitalWrite(GREEN_LED_PIN, HIGH);
    digitalWrite(RED_LED_PIN, LOW);
    LED.on();
  } else {
    Serial.println("No water => Tank is EMPTY");
    digitalWrite(GREEN_LED_PIN, LOW);
    digitalWrite(RED_LED_PIN, HIGH);
    LED.off();
  }
}

void loop() {
  sensor();
  Blynk.run();
  delay(200);
}
