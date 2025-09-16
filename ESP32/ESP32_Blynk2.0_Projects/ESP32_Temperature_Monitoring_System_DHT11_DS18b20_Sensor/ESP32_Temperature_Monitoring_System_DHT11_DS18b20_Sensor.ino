// Prateek
// JustDoElectronics
// Website: https://justdoelectronics.com
// YouTube: https://www.youtube.com/c/JustDoElectronics/videos

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

char auth[] = "xxxxxxxxxxxx";
char ssid[] = "xxxxxxxxxxxx";
char pass[] = "xxxxxxxxxxxx";

DHT dht(DHTPIN, DHTTYPE);
OneWire oneWire(DS18B20);
DallasTemperature sensors(&oneWire);

float Temperature;
float Humidity;
float bodytemperature;
uint32_t tsLastReport = 0;

void setup() {
  Serial.begin(115200);

  pinMode(DHTPIN, INPUT);
  pinMode(19, OUTPUT);  

  dht.begin();
  sensors.begin();
  
  Blynk.begin(auth, ssid, pass);

  Serial.println("System Initialized - Monitoring Room & Body Temperature");
}

void loop() {
  Blynk.run();

  Temperature = dht.readTemperature();
  Humidity = dht.readHumidity();

  sensors.requestTemperatures();
  bodytemperature = sensors.getTempCByIndex(0);

  if (millis() - tsLastReport > REPORTING_PERIOD_MS) {
    Serial.print("Room Temperature: ");
    Serial.print(Temperature);
    Serial.println(" °C");

    Serial.print("Room Humidity: ");
    Serial.print(Humidity);
    Serial.println(" %");

    Serial.print("Body Temperature: ");
    Serial.print(bodytemperature);
    Serial.println(" °C");

    // Send to Blynk app
    Blynk.virtualWrite(V1, Temperature);
    Blynk.virtualWrite(V2, Humidity);
    Blynk.virtualWrite(V5, bodytemperature);

    tsLastReport = millis();
  }
}
