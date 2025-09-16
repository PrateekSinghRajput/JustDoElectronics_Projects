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
#define IR_SENSOR_PIN 34
#define BUZZER_PIN 27

// Virtual pin for Blynk
#define VPIN_IR_SENSOR V1

void checkIRSensor() {
  int ir_state = digitalRead(IR_SENSOR_PIN);  // HIGH or LOW
  Serial.print("IR Sensor State: ");
  Serial.println(ir_state);

  if (ir_state == LOW) {
    digitalWrite(BUZZER_PIN, HIGH);         // Turn buzzer ON
    Blynk.virtualWrite(VPIN_IR_SENSOR, 1);  // Send alert to Blynk
    Blynk.logEvent("ir_alert", "⚠️ IR Sensor Triggered!");
  } else {
    digitalWrite(BUZZER_PIN, LOW);          // Turn buzzer OFF
    Blynk.virtualWrite(VPIN_IR_SENSOR, 0);  // Send safe status to Blynk
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(IR_SENSOR_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  // Connect to Blynk
  Blynk.begin(auth, ssid, pass);

  // Check IR sensor every second
  timer.setInterval(1000L, checkIRSensor);
}

void loop() {
  Blynk.run();
  timer.run();
}
