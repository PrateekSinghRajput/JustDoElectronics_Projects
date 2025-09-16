#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Pin Definitions
#define GSM_TX_PIN D5
#define GSM_RX_PIN D6

#define BUTTON_SMS_PIN D3
#define BUTTON_CALL_PIN D4

LiquidCrystal_I2C lcd(0x27, 16, 2);

TinyGPSPlus gps;
SoftwareSerial Gsm(GSM_RX_PIN, GSM_TX_PIN);

char phone_no[] = "+9188305848xx";

void setup() {
  Serial.begin(9600);
  Gsm.begin(9600);

  pinMode(BUTTON_SMS_PIN, INPUT_PULLUP);
  pinMode(BUTTON_CALL_PIN, INPUT_PULLUP);

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Initializing...");
  delay(2000);

  Gsm.println("AT+CMGF=1");
  delay(500);
  Gsm.println("AT+CNMI=2,2,0,0,0");
  delay(500);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Ready");
  delay(1000);
  lcd.clear();
}

void loop() {
  while (Serial.available() > 0) {
    gps.encode(Serial.read());
  }

  lcd.setCursor(0, 0);

  if (!gps.location.isValid()) {
    lcd.print("GPS No Fix     ");
  } else {
    lcd.print("Lat:");
    lcd.print(gps.location.lat(), 4);
    lcd.setCursor(0, 1);
    lcd.print("Lon:");
    lcd.print(gps.location.lng(), 4);
  }

  if (digitalRead(BUTTON_SMS_PIN) == LOW) {
    lcd.clear();
    lcd.print("Sending SMS...");
    sendGpsSms();
    lcd.clear();
    lcd.print("SMS Sent");
    delay(10000);
    lcd.clear();
  }

  if (digitalRead(BUTTON_CALL_PIN) == LOW) {
    lcd.clear();
    lcd.print("Making Call...");
    makeCall();
    lcd.clear();
    lcd.print("Call Ended");
    delay(150000);
    lcd.clear();
  }
}

void sendGpsSms() {
  if (!gps.location.isValid()) {
    lcd.print("No GPS Fix SMS");
    delay(2000);
    lcd.clear();
    return;
  }

  Gsm.println("AT+CMGF=1");
  delay(500);
  Gsm.print("AT+CMGS=\"");
  Gsm.print(phone_no);
  Gsm.println("\"");
  delay(500);

  Gsm.println("Alert! I need help.");
  Gsm.print("http://maps.google.com/maps?q=");
  Gsm.print(gps.location.lat(), 6);
  Gsm.print(",");
  Gsm.println(gps.location.lng(), 6);
  delay(500);
  Gsm.write(26);
  delay(1000);
}

void makeCall() {
  Gsm.print("ATD");
  Gsm.print(phone_no);
  Gsm.println(";");
  delay(150000);
  Gsm.println("ATH");
  delay(1000);
}
