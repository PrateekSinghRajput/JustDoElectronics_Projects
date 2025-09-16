// Author: Prateek
// www.justdoelectronics.com
// https://www.youtube.com/c/JustDoElectronics

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

#define SOIL_MOISTURE_PIN 34  
#define BUZZER_PIN 27         
#define LED_PIN  13         

#define VPIN_SOIL_MOISTURE V1

// Moisture threshold for dry soil (adjust according to calibration)
const int drySoilThreshold = 2000;

void sendSoilMoisture() {
  int soilMoistureValue = analogRead(SOIL_MOISTURE_PIN);

  Serial.print("Soil Moisture Raw Value: ");
  Serial.println(soilMoistureValue);

  // Send raw value to Blynk
  Blynk.virtualWrite(VPIN_SOIL_MOISTURE, soilMoistureValue);

  // Check moisture level and control buzzer and LED
  if (soilMoistureValue > drySoilThreshold) {
    // Soil is dry, alert on
    digitalWrite(BUZZER_PIN, HIGH);
    digitalWrite(LED_PIN, HIGH);
  } else {
    // Soil is wet, alert off
    digitalWrite(BUZZER_PIN, LOW);
    digitalWrite(LED_PIN, LOW);
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(SOIL_MOISTURE_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);

  digitalWrite(BUZZER_PIN, LOW);
  digitalWrite(LED_PIN, LOW);

  // Connect to Blynk
  Blynk.begin(auth, ssid, pass);

  // Check sensor every 1 second
  timer.setInterval(1000L, sendSoilMoisture);
}

void loop() {
  Blynk.run();
  timer.run();
}
void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}
