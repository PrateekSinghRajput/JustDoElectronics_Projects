// Author: Prateek
// www.justdoelectronics.com
// https://www.youtube.com/c/JustDoElectronics

#define BLYNK_TEMPLATE_ID "TMPLjOi0Rz-W"
#define BLYNK_DEVICE_NAME "esp32"
#define BLYNK_AUTH_TOKEN "UF3ok0_l0VkhTDS-WD1gWZ65DdhnHpwx"

#define BLYNK_PRINT Serial

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

// WiFi credentials
char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "prateeksingh";
char pass[] = "singh@@12345";

BlynkTimer timer;

// Pin Definitions
#define FLOAT_SENSOR_PIN 34
#define BUZZER_PIN 27

// Virtual pins for Blynk
#define VPIN_FLOAT V1

void checkFloatSensor() {
  int float_state = digitalRead(FLOAT_SENSOR_PIN);  // HIGH or LOW
  Serial.print("Float Sensor State: ");
  Serial.println(float_state);

  if (float_state == LOW) {             // Trigger condition: e.g., water low
    digitalWrite(BUZZER_PIN, HIGH);     // Turn buzzer ON
    Blynk.virtualWrite(VPIN_FLOAT, 1);  // Notify Blynk app
    Blynk.logEvent("float_alert", "⛔ Water Level Alert!");
  } else {
    digitalWrite(BUZZER_PIN, LOW);      // Turn buzzer OFF
    Blynk.virtualWrite(VPIN_FLOAT, 0);  // Safe
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(FLOAT_SENSOR_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  // Connect to Blynk
  Blynk.begin(auth, ssid, pass);

  // Run check every 1 second
  timer.setInterval(1000L, checkFloatSensor);
}

void loop() {
  Blynk.run();
  timer.run();
}
