#include <Wire.h>
#include <MAX30105.h>
#include <LiquidCrystal_PCF8574.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS D5  // Pin where the DS18B20 is connected
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

LiquidCrystal_PCF8574 lcd(0x27);

char auth[] = "uKprF7zKYAm-icZp-AEwInujTxnelyzj";
char ssid[] = "Prateek";
char pass[] = "justdoelectronics@#12345";

BlynkTimer timer;
MAX30105 particleSensor;

void setup() {
  Serial.begin(115200);
  Wire.begin();
  sensors.begin();
  lcd.begin(16, 2);
  lcd.setBacklight(255);
  Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);

  Serial.println("Initializing MAX30102 sensor...");
  if (!particleSensor.begin(Wire, I2C_SPEED_STANDARD)) {
    Serial.println("MAX30102 not detected. Please check wiring/power.");
    while (1);
  }
  particleSensor.setup();  // Initialize sensor with default settings
}

void loop() {
  Blynk.run();
  particleSensor.check();  // Poll the sensor for new data

  // Collect DS18B20 temperature data
  sensors.requestTemperatures(); 
  float temperature = sensors.getTempCByIndex(0);

  // Collect MAX30102 raw data
  float irValue = particleSensor.getIR();
  float redValue = particleSensor.getRed();

  // Limit the values to 3 digits after decimal
  irValue = (int)(irValue * 1) / 1000.0; // Keeps only 3 digits after decimal
  redValue = (int)(redValue * 1) / 1000.0; // Keeps only 3 digits after decimal

  // Display on LCD
  lcd.setCursor(0, 0);
  lcd.print("T:");
  lcd.print(temperature, 1);
  lcd.print("C");
  lcd.setCursor(8, 0);
  lcd.print("B:");
  lcd.print(irValue, 3);  // Display 3 digits after decimal
  lcd.setCursor(0, 1);
  lcd.print("SpO2:");
  lcd.print(redValue, 3);  // Display 3 digits after decimal

  // Send to Blynk
  Blynk.virtualWrite(V2, temperature);
  Blynk.virtualWrite(V0, redValue);
  Blynk.virtualWrite(V1, irValue);

  // Debug output
  Serial.print("Temp: ");
  Serial.print(temperature);
  Serial.print(" C, BPM: ");
  Serial.print(irValue);
  Serial.print(", Spo2: ");
  Serial.println(redValue);
  delay(700);
  lcd.clear();
}
