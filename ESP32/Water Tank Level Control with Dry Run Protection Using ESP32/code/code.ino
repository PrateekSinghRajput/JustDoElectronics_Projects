#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

const int buzzer = 12;
const int redLed = 13;
const int greenLed = 14;
const int FloatSensor = 27;

int buttonState = 1;

void setup() {
  pinMode(FloatSensor, INPUT_PULLUP);
  pinMode(redLed, OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(greenLed, OUTPUT);

  lcd.init();
  lcd.backlight();
  lcd.clear();

  Serial.begin(9600);

  lcd.setCursor(0, 0);
  lcd.print("WelCome To");
  lcd.setCursor(0, 1);
  lcd.print("JustDoElectronics");
  delay(2000);

  lcd.clear();
  lcd.setCursor(2, 0);
  lcd.print("WATER LEVEL");
}

void loop() {
  buttonState = digitalRead(FloatSensor);

  if (buttonState == LOW) {
    digitalWrite(redLed, LOW);
    digitalWrite(buzzer, LOW);
    digitalWrite(greenLed, HIGH);
    Serial.println("WATER LEVEL - Empty");

    lcd.setCursor(5, 1);
    lcd.print("Empty   ");
  } else {
    digitalWrite(redLed, HIGH);
    digitalWrite(buzzer, HIGH);
    digitalWrite(greenLed, LOW);
    Serial.println("WATER LEVEL - Full");

    lcd.setCursor(5, 1);
    lcd.print("Full    ");

    Serial.println("AT+CMGF=1");  
    delay(400);
    Serial.println();
    Serial.print("AT+CMGS=\"+9188305848xx\"");  
    Serial.println();
    delay(100);
    Serial.print("WATER LEVEL - Full");  
    delay(100);
    Serial.println();
    Serial.write(26);  
  }
  delay(500);  
}
