

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);
#include <SoftwareSerial.h>

SoftwareSerial mySerial(3, 2);

const int lm35_pin = A1;
int gasValue = A0;
int data = 0;
int relay = 10;
int pir_Status;
int buzzerPin = 9;

void setup()
{
  randomSeed(analogRead(0));
  mySerial.begin(9600);
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  pinMode(gasValue, INPUT);
  pinMode(relay, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  lcd.setCursor(0, 0);
  lcd.print ("  WELCOME TO  ");
  lcd.setCursor(0, 1);
  lcd.print ("  OUR PROJECT");
  delay(3000);
  lcd.clear();
}

void loop() {

  data = analogRead(gasValue);

  Serial.print("Gas Level: ");
  Serial.println(data);
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print("Gas Level = ");
  lcd.print(data);

  int temp_adc_val;
  float temp_val;
  temp_adc_val = analogRead(lm35_pin);  /* Read Temperature */
  temp_val = (temp_adc_val * 4.88); /* Convert adc value to equivalent voltage */
  temp_val = (temp_val / 10); /* LM35 gives output of 10mv/°C */
  Serial.print("Temperature = ");
  Serial.print(temp_val);
  Serial.print(" Degree Celsius\n");
  lcd.setCursor(0, 0);
  lcd.print("Temp = ");
  lcd.setCursor(9, 0);
  lcd.print(temp_val);
  delay(1000);

  if ( data > 20)
  {
    SendMessage();
    Serial.print("Gas detect alarm");
    delay(1000);
    digitalWrite(buzzerPin, HIGH);
    digitalWrite(relay, HIGH);

  }
  else
  {

    Serial.print("Gas Level Low");
    digitalWrite(buzzerPin, LOW);
    digitalWrite(relay, LOW);

  }

  //lcd.clear();
}

void SendMessage()
{
  Serial.println("I am in send");
  mySerial.println("AT+CMGF=1");
  delay(1000);
  mySerial.println("AT+CMGS=\"+91xxxxxxxxxx\"\r");
  delay(1000);
  mySerial.println("Gas Detected.PLZ Open Windows");
  delay(100);
  mySerial.println((char)26);
  delay(1000);
}
