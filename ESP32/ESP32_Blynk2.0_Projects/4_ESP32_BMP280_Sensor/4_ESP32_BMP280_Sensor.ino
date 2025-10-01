
#define BLYNK_TEMPLATE_ID "TMPLjOi0Rz-W"
#define BLYNK_DEVICE_NAME "esp32"
#define BLYNK_AUTH_TOKEN "UF3ok0_l0VkhTDS-WD1gWZ65DdhnHpwx"

#define BLYNK_PRINT Serial

#include <Wire.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <Adafruit_BMP280.h>

char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "prateeksingh";
char pass[] = "singh@@12345";

BlynkTimer timer;

Adafruit_BMP280 bmp;

#define VPIN_TEMPERATURE V1
#define VPIN_PRESSURE V2

#define LED_PIN 13
#define BUZZER_PIN 12

void sendSensorData() {
  float temperature = bmp.readTemperature();
  float pressure = bmp.readPressure() / 100.0F;

  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" °C");

  Serial.print("Pressure: ");
  Serial.print(pressure);
  Serial.println(" hPa");

  Blynk.virtualWrite(VPIN_TEMPERATURE, temperature);
  Blynk.virtualWrite(VPIN_PRESSURE, pressure);

  if (temperature > 30.0) {
    digitalWrite(LED_PIN, HIGH);
    digitalWrite(BUZZER_PIN, HIGH);
  } else {
    digitalWrite(LED_PIN, LOW);
    digitalWrite(BUZZER_PIN, LOW);
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  if (!bmp.begin(0x76)) {
    Serial.println("Could not find BMP280 sensor, check wiring.");
    while (1) delay(10);
  }

  Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);

  timer.setInterval(1000L, sendSensorData);
}

void loop() {
  Blynk.run();
  timer.run();
}
