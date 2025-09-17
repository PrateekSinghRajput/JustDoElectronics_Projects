#define BLYNK_PRINT Serial
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

#define DE_RE 4  
#define SOIL_SENSOR_PIN 34  

HardwareSerial rs485(1);  
LiquidCrystal_I2C lcd(0x27, 16, 2); 

char auth[] = "EHZVcotxyg6Z0oZiscdsjhk7ihlz_";
char ssid[] = "Prateek";
char pass[] = "justdoelectronic5";

const byte nitro[] = {0x01, 0x03, 0x00, 0x1E, 0x00, 0x01, 0xE4, 0x0C};
const byte phos[]  = {0x01, 0x03, 0x00, 0x1F, 0x00, 0x01, 0xB5, 0xCC};
const byte pota[]  = {0x01, 0x03, 0x00, 0x20, 0x00, 0x01, 0x85, 0xC0};

byte values[7];

void setup() {
  Serial.begin(115200);
  rs485.begin(9600, SERIAL_8N1, 16, 17); 
  
  pinMode(DE_RE, OUTPUT);
  digitalWrite(DE_RE, LOW); 
  
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("NPK Sensor");
  lcd.setCursor(0, 1);
  lcd.print("Initializing...");
  delay(2000);
  
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Connected");
  Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);
}

void loop() {
  
  Blynk.run();
  byte val1 = readSensor(nitro);
  delay(250);
  byte val2 = readSensor(phos);
  delay(250);
  byte val3 = readSensor(pota);
  delay(250);
  int soilMoisture = analogRead(SOIL_SENSOR_PIN);
  
  Serial.printf("Nitrogen: %d mg/kg\n", val1);
  Serial.printf("Phosphorous: %d mg/kg\n", val2);
  Serial.printf("Potassium: %d mg/kg\n", val3);
  Serial.printf("Soil Moisture: %d\n", soilMoisture);
  Serial.println("---------------------------");
  
  displayValues(val1, val2, val3, soilMoisture);
  
  Blynk.virtualWrite(V0, val1);
  Blynk.virtualWrite(V1, val2);
  Blynk.virtualWrite(V2, val3);
  Blynk.virtualWrite(V3, soilMoisture);
  
  delay(2000);
}

byte readSensor(const byte* cmd) {
  digitalWrite(DE_RE, HIGH);  
  delay(10);
  rs485.write(cmd, 8);
  delay(10);
  digitalWrite(DE_RE, LOW);   

  int index = 0;
  unsigned long startTime = millis();
  while (millis() - startTime < 200) {  
    if (rs485.available()) {
      values[index++] = rs485.read();
      if (index >= 7) break;  
    }
  }

  if (index < 7) {
    Serial.println("Error: Incomplete response");
    return 0;
  }

  return values[4];  
}

void displayValues(byte n, byte p, byte k, int moisture) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("N:"); lcd.print(n); lcd.print(" P:"); lcd.print(p); lcd.print(" K:"); lcd.print(k);
  lcd.setCursor(0, 1);
  lcd.print("mg/kg"); lcd.print("   M:"); lcd.print(moisture);
}
