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
#define FLAME_SENSOR_PIN 34  // Use analog input (GPIO34)
#define BUZZER_PIN 27        // Buzzer on GPIO27

// Virtual pins for Blynk
#define VPIN_FLAME V1  // For flame sensor status display (1=Fire, 0=Safe)

int flame_threshold = 500;  // Adjust threshold depending on sensor

void checkFlameSensor() {
  int flame_value = analogRead(FLAME_SENSOR_PIN);
  Serial.print("Flame Sensor Value: ");
  Serial.println(flame_value);

  if (flame_value < flame_threshold) {
    // Fire Detected
    digitalWrite(BUZZER_PIN, HIGH);
    Blynk.virtualWrite(VPIN_FLAME, 1);
    Blynk.logEvent("fire_alert", "🔥 Fire Detected! Take Action!");
  } else {
    // No Fire
    digitalWrite(BUZZER_PIN, LOW);
    Blynk.virtualWrite(VPIN_FLAME, 0);
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(FLAME_SENSOR_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  // Connect to Blynk
  Blynk.begin(auth, ssid, pass);

  // Run check every 1 second
  timer.setInterval(1000L, checkFlameSensor);
}

void loop() {
  Blynk.run();
  timer.run();
}
