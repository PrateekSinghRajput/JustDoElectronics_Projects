#define BLYNK_TEMPLATE_ID "TMPLjOi0Rz-W"
#define BLYNK_DEVICE_NAME "esp32"
#define BLYNK_AUTH_TOKEN "UF3ok0_l0VkhTDS-W"

#define BLYNK_PRINT Serial

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "prateeksingh";
char pass[] = "singh@@12345";

BlynkTimer timer;

#define SOIL_MOISTURE_PIN 32
#define BUZZER_PIN 27
#define RED_LED_PIN 13
#define GREEN_LED_PIN 14

#define VPIN_SOIL_MOISTURE V1

const int drySoilThreshold = 2000;

void sendSoilMoisture() {
  int soilMoistureValue = analogRead(SOIL_MOISTURE_PIN);

  Serial.print("Soil Moisture Raw Value: ");
  Serial.println(soilMoistureValue);

  Blynk.virtualWrite(VPIN_SOIL_MOISTURE, soilMoistureValue);

  if (soilMoistureValue > drySoilThreshold) {
    digitalWrite(BUZZER_PIN, HIGH);
    digitalWrite(RED_LED_PIN, HIGH);
    digitalWrite(GREEN_LED_PIN, LOW);
  } else {
    digitalWrite(BUZZER_PIN, LOW);
    digitalWrite(RED_LED_PIN, LOW);
    digitalWrite(GREEN_LED_PIN, HIGH);
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(SOIL_MOISTURE_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);

  digitalWrite(BUZZER_PIN, LOW);
  digitalWrite(RED_LED_PIN, LOW);
  digitalWrite(GREEN_LED_PIN, LOW);

  Blynk.begin(auth, ssid, pass);

  timer.setInterval(1000L, sendSoilMoisture);
}

void loop() {
  Blynk.run();
  timer.run();
}
