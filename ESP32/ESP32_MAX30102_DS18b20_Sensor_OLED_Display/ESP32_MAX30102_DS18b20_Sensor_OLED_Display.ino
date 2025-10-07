#include <Wire.h>
#include <MAX30105.h>
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_ADDR 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

#define ONE_WIRE_BUS 12
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

char auth[] = "klxL5aJx1pzaCAetK03Tx3m7Mp2byS1d";
char ssid[] = "Prateek";
char pass[] = "justdoelectronics@#12345";

MAX30105 particleSensor;

#define BATTERY_PIN 34
const float R1 = 10000.0;
const float R2 = 10000.0;

// Convert Celsius to Fahrenheit
float celsiusToFahrenheit(float celsius) {
  return (celsius * 9.0 / 5.0) + 32.0;
}

// Function to read battery voltage via ADC and voltage divider
float getBatteryVoltage() {
  int adcValue = analogRead(BATTERY_PIN);
  float v = ((float)adcValue / 4095.0) * 3.3; // ADC to voltage
  float batteryVoltage = v * ((R1 + R2) / R2); // Scale by voltage divider ratio
  return batteryVoltage;
}

// Convert battery voltage to percentage 0-100%
// Assumes Li-ion battery: 3.0V = 0%, 4.2V = 100%
int getBatteryPercentage(float voltage) {
  float minVolt = 3.0;
  float maxVolt = 4.2;
  int percent = (int)(((voltage - minVolt) / (maxVolt - minVolt)) * 100.0);
  if (percent > 100) percent = 100;
  if (percent < 0) percent = 0;
  return percent;
}

// Draw all values on OLED screen
void drawOLED(float temperatureF, float irDisp, float redDisp, int batteryPercent) {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("Temp: ");
  display.print(temperatureF, 1);
  display.print(" F");

  display.setCursor(0, 16);
  display.print("BPM: ");
  display.print(irDisp, 3);

  display.setCursor(0, 32);
  display.print("SpO2: ");
  display.print(redDisp, 3);

  display.setCursor(0, 48);
  display.print("Batt: ");
  display.print(batteryPercent);
  display.print(" %");

  display.display();
}

void setup() {
  Serial.begin(115200);
  Wire.begin();

  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println("SSD1306 OLED not found. Check wiring/address.");
    while (1) { delay(10); }
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Booting...");
  display.display();

  sensors.begin();
  Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);

  pinMode(BATTERY_PIN, INPUT);

  Serial.println("Initializing MAX30102 sensor...");
  if (!particleSensor.begin(Wire, I2C_SPEED_STANDARD)) {
    Serial.println("MAX30102 not detected. Please check wiring/power.");
    while (1);
  }
  particleSensor.setup();

  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Ready");
  display.display();
}

void loop() {
  Blynk.run();

  particleSensor.check();

  sensors.requestTemperatures();
  float temperatureC = sensors.getTempCByIndex(0);
  float temperatureF = celsiusToFahrenheit(temperatureC);

  float irValue = particleSensor.getIR();
  float redValue = particleSensor.getRed();

  irValue = (int)(irValue * 1) / 1000.0;
  redValue = (int)(redValue * 1) / 1000.0;

  float batteryVoltage = getBatteryVoltage();
  int batteryPercent = getBatteryPercentage(batteryVoltage);

  drawOLED(temperatureF, irValue, redValue, batteryPercent);

  Blynk.virtualWrite(V0, temperatureF);
  Blynk.virtualWrite(V1, irValue);
  Blynk.virtualWrite(V2, redValue);
  Blynk.virtualWrite(V3, batteryPercent);

  Serial.print("IR: ");
  Serial.print(irValue);
  Serial.print("\tRED: ");
  Serial.print(redValue);
  Serial.print("\tTEMP: ");
  Serial.print(temperatureF, 1);
  Serial.print(" F\tBATTERY: ");
  Serial.print(batteryPercent);
  Serial.println(" %");

  delay(700);
}
