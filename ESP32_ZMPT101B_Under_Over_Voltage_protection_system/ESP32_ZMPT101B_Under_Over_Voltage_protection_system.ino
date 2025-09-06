#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <ZMPT101B.h>
#include <WiFi.h> 
#include <BlynkSimpleEsp32.h> 

LiquidCrystal_I2C lcd(0x27, 16, 2);

#define SENSITIVITY 500.0f
#define RELAY_PIN 14 

// ZMPT101B sensor output connected to ADC pin 34
// and the voltage source frequency is 50 Hz.
ZMPT101B voltageSensor(34, 50.0);


char auth[] = "svr-hSXJajBDcey6VtZCNmdBaUyKir"; 
char ssid[] = "Prateek";      
char pass[] = "ju12345";  

// Virtual pins for Blynk app
#define VIRTUAL_PIN V0 // For displaying voltage
#define LED_UNDER_VOLTAGE V2 // For under-voltage LED
#define LED_OVER_VOLTAGE V1  // For over-voltage LED

void setup() {

  Serial.begin(115200);

  Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);

  // Change the sensitivity value based on value you got from the calibrate example
  voltageSensor.setSensitivity(SENSITIVITY);

  lcd.init();   
  lcd.backlight();  

  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW); 

  lcd.setCursor(0, 0);
  lcd.print("Voltage Monitor");
  delay(2000);
  lcd.clear();
}

void loop() {

  Blynk.run(); 

  // Read the voltage and then print via Serial
  float voltage = voltageSensor.getRmsVoltage();
  Serial.println(voltage);

  // Send voltage to Blynk app
  Blynk.virtualWrite(VIRTUAL_PIN, voltage);

  delay(500);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Voltage: ");
  lcd.print(voltage);
  lcd.print("V"); 

  // Check for under-voltage or over-voltage conditions
  if (voltage < 180.0) {
    lcd.setCursor(0, 1);
    lcd.print("Under Voltage ");
    digitalWrite(RELAY_PIN, LOW); 
    Blynk.notify("Under Voltage detected!"); // Send notification to Blynk app
    Blynk.virtualWrite(LED_UNDER_VOLTAGE, 255); // Turn on under-voltage LED
    Blynk.virtualWrite(LED_OVER_VOLTAGE, 0);    // Turn off over-voltage LED
  } else if (voltage > 242.0) {
    lcd.setCursor(0, 1);
    lcd.print("Over Voltage  ");
    digitalWrite(RELAY_PIN, LOW); 
    Blynk.notify("Over Voltage detected!"); // Send notification to Blynk app
    Blynk.virtualWrite(LED_UNDER_VOLTAGE, 0);    // Turn off under-voltage LED
    Blynk.virtualWrite(LED_OVER_VOLTAGE, 255);   // Turn on over-voltage LED
  } else {
    lcd.setCursor(0, 1);
    lcd.print("Normal         ");
    digitalWrite(RELAY_PIN, HIGH); 
    Blynk.virtualWrite(LED_UNDER_VOLTAGE, 0);    // Turn off under-voltage LED
    Blynk.virtualWrite(LED_OVER_VOLTAGE, 0);     // Turn off over-voltage LED
  }
}
