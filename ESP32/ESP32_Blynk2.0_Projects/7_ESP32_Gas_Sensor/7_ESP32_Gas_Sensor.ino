#include <Wire.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

#define MQ135_PIN 32
#define RED_LED_PIN 12
#define GREEN_LED_PIN 27
#define BUZZER_PIN 14

char auth[] = "xxxxxxxxxxxx";
char ssid[] = "Prateek";
char pass[] = "12345@#12345";

BlynkTimer timer;

#define VPIN_GAS_LEVEL V1

const int gasThreshold = 3000;

void sendGasSensorData() {
  int gasValue = analogRead(MQ135_PIN);

  Serial.print("MQ135 Gas sensor raw value: ");
  Serial.println(gasValue);

  Blynk.virtualWrite(VPIN_GAS_LEVEL, gasValue);

  if (gasValue > gasThreshold) {
    digitalWrite(RED_LED_PIN, HIGH);
    digitalWrite(GREEN_LED_PIN, LOW);
    digitalWrite(BUZZER_PIN, HIGH);
  } else {
    digitalWrite(RED_LED_PIN, LOW);
    digitalWrite(GREEN_LED_PIN, HIGH);
    digitalWrite(BUZZER_PIN, LOW);
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(MQ135_PIN, INPUT);
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  digitalWrite(RED_LED_PIN, LOW);
  digitalWrite(GREEN_LED_PIN, LOW);
  digitalWrite(BUZZER_PIN, LOW);

  Blynk.begin(auth, ssid, pass);

  timer.setInterval(1000L, sendGasSensorData);
}

void loop() {
  Blynk.run();
  timer.run();
}
