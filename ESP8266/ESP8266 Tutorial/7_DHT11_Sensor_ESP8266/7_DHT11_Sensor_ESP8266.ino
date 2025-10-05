#include <Arduino.h>
#include <DHT.h>

#define DHTPIN 5  //D1

#define DHTTYPE DHT11  // DHT 11
//#define DHTTYPE DHT22 // DHT 22

DHT dht(DHTPIN, DHTTYPE);
float t = 0.0;
float h = 0.0;


void setup() {

  Serial.begin(115200);
  dht.begin();
}

void loop() {

  float newT = dht.readTemperature();
  if (isnan(newT)) {
    Serial.println("Failed to read from DHT sensor!");
  } else {
    t = newT;
    Serial.println("Tempature:");
    Serial.println(t);
  }
  float newH = dht.readHumidity();
  if (isnan(newH)) {
    Serial.println("Failed to read from DHT sensor!");
  } else {
    h = newH;
    Serial.println("Humditiy:");
    Serial.println(h);
  }
  delay(1000);
}

