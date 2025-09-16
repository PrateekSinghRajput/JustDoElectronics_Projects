#include <ESP8266WiFi.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <SoftwareSerial.h>


SoftwareSerial gsmSerial(D6, D7);

LiquidCrystal_I2C lcd(0x27, 16, 2);

int FloatSensor = D8;  //A0 pin
int buttonState = 1;
const int trigPin1 = D4;
const int echoPin1 = D3;

const String alertNumber = "+91xxxxxxxxxx";
const String rescueNumber = "+91xxxxxxxxxx";

unsigned long startMillis;
unsigned long currentMillis;
const unsigned long period = 10000;
long duration1;
int distance1;

bool alertSent_FloatSensor = false;
bool alertSent_Distance = false;

void setup() {
  Serial.begin(9600);
  gsmSerial.begin(9600);
  Wire.begin(D2, D1);
  pinMode(FloatSensor, INPUT_PULLUP);

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print(" WELCOME TO");
  lcd.setCursor(0, 1);
  lcd.print(" OUR PROJECTS");
  delay(2000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(" SMART");
  lcd.setCursor(0, 1);
  lcd.print(" DAM MONITORING");
  delay(3000);
  lcd.clear();
  pinMode(trigPin1, OUTPUT);
  pinMode(echoPin1, INPUT);
  startMillis = millis();
}

void loop() {

  digitalWrite(trigPin1, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin1, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin1, LOW);
  duration1 = pulseIn(echoPin1, HIGH);
  distance1 = duration1 * 0.034 / 2;
  Serial.print("Distance: ");
  Serial.println(distance1);

  buttonState = digitalRead(FloatSensor);


  if (buttonState == HIGH) {
    if (!alertSent_FloatSensor) {
      Serial.println("WATER LEVEL - HIGH");
      sendMessage(alertNumber, "Flood Alert! Water level has crossed the danger limit.");
      sendMessage(rescueNumber, "URGENT RESCUE ALERT: Water level at the dam is critical. Immediate assistance required.");
      alertSent_FloatSensor = true;
    }

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(" HIGH ALERTS");
    lcd.setCursor(0, 1);
    lcd.print("WATER LEVEL-");
    lcd.setCursor(13, 1);
    lcd.print(distance1);
  } else {
    alertSent_FloatSensor = false;


    Serial.println("WATER LEVEL - LOW");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(" LOW ALERTS");
    lcd.setCursor(0, 1);
    lcd.print("WATER LEVEL-");
    lcd.setCursor(13, 1);
    lcd.print(distance1);
    delay(300);
  }

  if (distance1 < 10) {
    if (!alertSent_Distance) {
      sendMessage(alertNumber, "Flood Alert! Water level has crossed the 10% danger limit.");
      sendMessage(rescueNumber, "RESCUE TEAM: Water level is critically high (10% over threshold). Prepare for deployment.");
      alertSent_Distance = true;
    }
  } else {
    alertSent_Distance = false;
  }
}

void sendMessage(String phoneNumber, String message) {
  Serial.print("Attempting to send SMS to ");
  Serial.println(phoneNumber);

  gsmSerial.println("AT+CMGF=1");
  delay(1000);

  gsmSerial.print("AT+CMGS=\"");
  gsmSerial.print(phoneNumber);
  gsmSerial.println("\"");
  delay(1000);

  if (gsmSerial.find(">")) {
    Serial.println("Module ready to send message...");
    gsmSerial.println(message);
    delay(100);
    gsmSerial.write((char)26);
    delay(2000);

    if (gsmSerial.find("OK")) {
      Serial.println("SMS sent successfully.");
    } else {
      Serial.println("SMS failed to send. GSM module did not return 'OK'.");
    }
  } else {
    Serial.println("GSM module not ready. Failed to send message.");
  }
}