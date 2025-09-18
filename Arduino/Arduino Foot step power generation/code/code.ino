#include <Wire.h>
#include <LiquidCrystal_I2C.h>

const int piezoPin = A0;
const int threshold = 100; 

const unsigned long debounceDelay = 250; 

int stepCount = 0;
bool lastState = false; 
unsigned long lastStepTime = 0;

LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  lcd.init();
  lcd.backlight();
  lcd.print("Footstep Enegy");
  lcd.setCursor(3, 1);
  lcd.print("Generator");
  delay(1500);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Steps: 0");
  lcd.setCursor(0, 1);
  lcd.print("Voltage: 0mV");
}

void loop() {
  int sensorValue = analogRead(piezoPin);
  float vout_mv = ((sensorValue * 5.0) / 1023.0) * 1000.0; 

  lcd.setCursor(9, 1);
  lcd.print((int)vout_mv);
  lcd.print("mV  "); 

  bool currentState = (sensorValue > threshold);
  unsigned long now = millis();

 
  if (currentState && !lastState && (now - lastStepTime > debounceDelay)) {
    stepCount++;
    lastStepTime = now;
    lcd.setCursor(7, 0);
    lcd.print(stepCount);
  }

  lastState = currentState; 
}