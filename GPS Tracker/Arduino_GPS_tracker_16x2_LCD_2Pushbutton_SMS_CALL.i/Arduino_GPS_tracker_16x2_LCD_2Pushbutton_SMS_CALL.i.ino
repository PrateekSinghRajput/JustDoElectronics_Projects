#include <TinyGPS.h>
#include <SoftwareSerial.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

SoftwareSerial Gsm(2, 3);
TinyGPS gps;

char phone_no[] = "+9188305848xx";

const int buttonSmsPin = 5;
const int buttonCallPin = 6;

LiquidCrystal_I2C lcd(0x27, 16, 2); 

void setup() {
  Serial.begin(9600);
  Gsm.begin(9600);

  pinMode(buttonSmsPin, INPUT_PULLUP);
  pinMode(buttonCallPin, INPUT_PULLUP);

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

  // Show GPS status on LCD
  float lat, lon;
  unsigned long age;
  gps.f_get_position(&lat, &lon, &age);

  lcd.setCursor(0, 0);
  if (lat == TinyGPS::GPS_INVALID_F_ANGLE || lon == TinyGPS::GPS_INVALID_F_ANGLE) {
    lcd.print("GPS No Fix     ");
  } else {
    lcd.print("Lat:");
    lcd.print(lat, 4);
    lcd.setCursor(0, 1);
    lcd.print("Lon:");
    lcd.print(lon, 4);
  }

  if (digitalRead(buttonSmsPin) == LOW) {
    lcd.clear();
    lcd.print("Sending SMS...");
    sendGpsSms();
    lcd.clear();
    lcd.print("SMS Sent");
    delay(10000);
    lcd.clear();
  }

  if (digitalRead(buttonCallPin) == LOW) {
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
  float lat, lon;
  unsigned long age;

  gps.f_get_position(&lat, &lon, &age);

  Gsm.println("AT+CMGF=1");
  delay(500);
  Gsm.print("AT+CMGS=\"");
  Gsm.print(phone_no);
  Gsm.println("\"");
  delay(500);

  Gsm.println("Alert! I need help.");
  Gsm.print("http://maps.google.com/maps?q=");
  Gsm.print(lat == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : lat, 6);
  Gsm.print(",");
  Gsm.println(lon == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : lon, 6);

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
