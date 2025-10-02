#include <LiquidCrystal_I2C.h>
#include <Wire.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

int lm35Pin = A0;
float tempThreshold = 30;

int Buzzer = 5;
int Relay = 3;

bool alertSent = false;
bool systemStartSent = false;

void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();

  pinMode(lm35Pin, INPUT);
  pinMode(Buzzer, OUTPUT);
  pinMode(Relay, OUTPUT);

  digitalWrite(Buzzer, LOW);
  digitalWrite(Relay, LOW);

  lcd.print("   WELCOME TO");
  lcd.setCursor(0, 1);
  lcd.print(" JSUTDOELECTRONIC");
  delay(3000);
  lcd.clear();

  lcd.print("  Temperature  ");
  lcd.setCursor(0, 1);
  lcd.print("    Monitor   ");
  delay(3000);
  lcd.clear();

  if (!systemStartSent) {
    SendSystemStartSMS();
    systemStartSent = true;
  }
}

void loop() {
  int analogValue = analogRead(lm35Pin);
  float voltage = analogValue * (5.0 / 1023.0);
  float temperatureC = voltage * 100;

  Serial.print("Temperature: ");
  Serial.print(temperatureC);
  Serial.println(" °C");

  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.print(temperatureC, 1);
  lcd.print(" C   ");

  if (temperatureC > tempThreshold) {
    if (!alertSent) {
      SendTemperatureAlertSMS(temperatureC);
      alertSent = true;
    }
    lcd.setCursor(0, 1);
    lcd.print("ALERT! High Temp!");

    digitalWrite(Buzzer, HIGH);
    digitalWrite(Relay, HIGH);
  } else {
    alertSent = false;
    lcd.setCursor(0, 1);
    lcd.print("Temp Normal      ");

    digitalWrite(Buzzer, LOW);
    digitalWrite(Relay, LOW);
  }

  delay(2000);
  lcd.clear();
}

void SendSystemStartSMS() {
  Serial.println("AT+CMGF=1");
  delay(1000);
  Serial.println("AT+CMGS=\"+9188305848xx\"\r");
  delay(1000);
  Serial.println("Temperature Monitor System is ON. Tracking temperature.");
  delay(100);
  Serial.write((char)26);
  delay(1000);
}

void SendTemperatureAlertSMS(float temp) {
  Serial.println("AT+CMGF=1");
  delay(1000);
  Serial.println("AT+CMGS=\"+9188305848xx\"\r");
  delay(1000);
  Serial.print("Warning! High Temperature: ");
  Serial.print(temp, 1);
  Serial.println(" C detected!");
  delay(100);
  Serial.write((char)26);
  delay(1000);
}
