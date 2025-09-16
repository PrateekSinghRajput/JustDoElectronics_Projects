// Prateek
// JustDoElectronics
// https://justdoelectronics.com
// https://www.youtube.com/c/JustDoElectronics/videos

#include <Wire.h>
#include "MAX30100_PulseOximeter.h"
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "DHT.h"

#define BLYNK_PRINT Serial
#define DHTPIN 18
#define DHTTYPE DHT11
#define DS18B20 5
#define REPORTING_PERIOD_MS 1000

char auth[] = "G9nCH4W7AWUplzwpGLleH0fOdXF9_";
char ssid[] = "justdoelectronics";
char pass[] = "123456789";


PulseOximeter pox;
DHT dht(DHTPIN, DHTTYPE);
OneWire oneWire(DS18B20);
DallasTemperature sensors(&oneWire);


float Temperature;
float Humidity;
float bodytemperature;
float BPM, SpO2;
uint32_t tsLastReport = 0;

void onBeatDetected() {
  Serial.println("Beat Detected!");
}

void setup() {
  Serial.begin(115200);
  pinMode(DHTPIN, INPUT);
  pinMode(19, OUTPUT);  

  dht.begin();
  sensors.begin();

  Blynk.begin(auth, ssid, pass);

  Serial.print("Initializing Pulse Oximeter...");
  if (!pox.begin()) {
    Serial.println("FAILED");
    while (1); 
  } else {
    Serial.println("SUCCESS");
    pox.setOnBeatDetectedCallback(onBeatDetected);
  }
  pox.setIRLedCurrent(MAX30100_LED_CURR_7_6MA);
}

void loop() {
  pox.update();
  Blynk.run();

  Temperature = dht.readTemperature();
  Humidity = dht.readHumidity();
  BPM = pox.getHeartRate();
  SpO2 = pox.getSpO2();

  sensors.requestTemperatures();
  bodytemperature = sensors.getTempCByIndex(0);

  if (millis() - tsLastReport > REPORTING_PERIOD_MS) {
    Serial.print("Heart rate: ");
    Serial.print(BPM);
    Serial.print(" bpm / SpO2: ");
    Serial.print(SpO2);
    Serial.println(" %");

    Serial.print("Room Temperature: ");
    Serial.print(Temperature);
    Serial.println(" °C");

    Serial.print("Room Humidity: ");
    Serial.print(Humidity);
    Serial.println(" %");

    Serial.print("Body Temperature: ");
    Serial.print(bodytemperature);
    Serial.println(" °C");

    Blynk.virtualWrite(V1, Temperature);
    Blynk.virtualWrite(V2, Humidity);
    Blynk.virtualWrite(V3, BPM);
    Blynk.virtualWrite(V4, SpO2);
    Blynk.virtualWrite(V5, bodytemperature);

    tsLastReport = millis();
  }
}
