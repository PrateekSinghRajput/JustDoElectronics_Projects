#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <math.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

const String EMERGENCY_PHONE = "+9188305848xx";

#define SIM800_RX_PIN 2
#define SIM800_TX_PIN 3
SoftwareSerial sim800Serial(SIM800_RX_PIN, SIM800_TX_PIN);

#define GPS_RX_PIN 8
#define GPS_TX_PIN 9
SoftwareSerial gpsSerial(GPS_RX_PIN, GPS_TX_PIN);

TinyGPSPlus gps;

#define xPin A0
#define yPin A1
#define zPin A2

#define BUZZER 5
#define BUTTON 6

byte updateflag = 0;
int xaxis = 0, yaxis = 0, zaxis = 0;
int deltx = 0, delty = 0, deltz = 0;
int vibration = 2, devibrate = 75;
int magnitude = 0;
int sensitivity = 160;
boolean impact_detected = false;
unsigned long time1;
unsigned long buzzerStartTime;
const unsigned long buzzerDuration = 20000;
String latitude, longitude;
String simBuffer = "";
bool readingSMS = false;
String smsSender = "";
String smsContent = "";

void setup() {
  Serial.begin(115200);
  sim800Serial.begin(9600);
  gpsSerial.begin(9600);

  pinMode(BUZZER, OUTPUT);
  pinMode(BUTTON, INPUT_PULLUP);

  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("System Starting...");

  sendAT(sim800Serial, "AT", "OK", 1000);
  sendAT(sim800Serial, "ATE1", "OK", 1000);
  sendAT(sim800Serial, "AT+CPIN?", "READY", 2000);
  sendAT(sim800Serial, "AT+CMGF=1", "OK", 1000);
  sendAT(sim800Serial, "AT+CNMI=2,2,0,0,0", "OK", 1000);

  delay(2000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Ready");

  time1 = micros();
  xaxis = analogRead(xPin);
  yaxis = analogRead(yPin);
  zaxis = analogRead(zPin);
}

void loop() {
  if (micros() - time1 > 1999) Impact();

  if (updateflag > 0) {
    updateflag = 0;
    Serial.println("Impact detected!!");
    Serial.print("Magnitude: ");
    Serial.println(magnitude);
    getGps();
    digitalWrite(BUZZER, HIGH);
    impact_detected = true;
    buzzerStartTime = millis();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Crash Detected");
    lcd.setCursor(0, 1);
    lcd.print("Magnitude: " + String(magnitude));
  }

  if (impact_detected) {
    unsigned long elapsed = millis() - buzzerStartTime;
    unsigned long remaining = (elapsed < buzzerDuration) ? (buzzerDuration - elapsed) / 1000 : 0;
    lcd.setCursor(0, 1);
    if (remaining > 0) {
      lcd.print("Cancel Btn in: ");
      lcd.print(remaining);
      lcd.print("s ");
    } else {
      lcd.print("Sending alert...  ");
    }
    if (elapsed >= buzzerDuration) {
      digitalWrite(BUZZER, LOW);
      makeCall();
      delay(1000);
      sendAlert();
      impact_detected = false;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Alert Sent");
      lcd.setCursor(0, 1);
      lcd.print("Help is coming!");
      delay(5000);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Ready");
    }
  }

  if (digitalRead(BUTTON) == LOW && impact_detected) {
    delay(200);
    digitalWrite(BUZZER, LOW);
    impact_detected = false;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Alert Cancelled");
    lcd.setCursor(0, 1);
    lcd.print("Stay Safe");
    Serial.println("Alert cancelled by user.");
    delay(3000);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Ready");
  }

  while (sim800Serial.available()) {
    char c = (char)sim800Serial.read();
    Serial.write(c);
    if (c == '\r' || c == '\n') {
      simBuffer.trim();
      if (simBuffer.length() > 0) {
        processSIMLine(simBuffer);
        simBuffer = "";
      }
    } else {
      simBuffer += c;
    }
  }

  while (Serial.available()) {
    sim800Serial.write(Serial.read());
  }
}

void processSIMLine(String line) {
  if (line.startsWith("+CMT:")) {
    int firstQuote = line.indexOf('\"');
    int secondQuote = line.indexOf('\"', firstQuote + 1);
    int thirdQuote = line.indexOf('\"', secondQuote + 1);
    int fourthQuote = line.indexOf('\"', thirdQuote + 1);
    if (secondQuote != -1 && fourthQuote != -1) {
      smsSender = line.substring(thirdQuote + 1, fourthQuote);
      smsContent = "";
      readingSMS = true;
      Serial.println("SMS from: " + smsSender);
    }
  } else if (readingSMS) {
    if (line.length() == 0) {
      readingSMS = false;
      handleSMS(smsSender, smsContent);
    } else {
      smsContent += line + "\n";
    }
  }
}

void handleSMS(String sender, String message) {
  Serial.println("Complete SMS Received:");
  Serial.println("From: " + sender);
  Serial.println("Message: " + message);

  if (sender == EMERGENCY_PHONE) {
    String msgLower = message;
    msgLower.toLowerCase();
    if (msgLower.indexOf("getloc") >= 0) {
      getGps();
      delay(500);
      String sms_data = "GPS Location Data\r\n";
      sms_data += "http://maps.google.com/maps?q=loc:";
      sms_data += latitude + "," + longitude;
      sendSms(sms_data);
    }
  }
}

void getGps() {
  bool newData = false;
  unsigned long start = millis();
  while (millis() - start < 2000) {
    while (gpsSerial.available()) {
      if (gps.encode(gpsSerial.read())) {
        newData = true;
        break;
      }
    }
    if (newData) break;
  }

  if (newData && gps.location.isValid()) {
    latitude = String(gps.location.lat(), 6);
    longitude = String(gps.location.lng(), 6);
  } else {
    Serial.println("No GPS signal, using fallback location.");
    latitude = "18.5849324";
    longitude = "73.6553194";
  }

  Serial.print("Latitude= ");
  Serial.println(latitude);
  Serial.print("Longitude= ");
  Serial.println(longitude);
}

void sendSms(String text) {
  sim800Serial.println("AT+CMGF=1");
  delay(1000);
  sim800Serial.print("AT+CMGS=\"" + EMERGENCY_PHONE + "\"\r");
  delay(1000);
  sim800Serial.print(text);
  delay(100);
  sim800Serial.write(0x1A);
  delay(1000);
  Serial.println("SMS Sent Successfully.");
}

void makeCall() {
  Serial.println("calling...");
  sim800Serial.println("ATD" + EMERGENCY_PHONE + ";");
  delay(20000);
  sim800Serial.println("ATH");
  delay(1000);
}

void sendAlert() {
  String sms_data = "Accident Alert!!\r\n";
  sms_data += "http://maps.google.com/maps?q=loc:";
  sms_data += latitude + "," + longitude;
  sendSms(sms_data);
}

void Impact() {
  time1 = micros();
  int oldx = xaxis, oldy = yaxis, oldz = zaxis;
  xaxis = analogRead(xPin);
  yaxis = analogRead(yPin);
  zaxis = analogRead(zPin);
  vibration--;
  if (vibration < 0) vibration = 0;
  if (vibration > 0) return;
  deltx = xaxis - oldx;
  delty = yaxis - oldy;
  deltz = zaxis - oldz;
  magnitude = sqrt(sq(deltx) + sq(delty) + sq(deltz));
  if (magnitude >= sensitivity) {
    updateflag = 1;
    vibration = devibrate;
  } else {
    magnitude = 0;
  }
}

bool sendAT(SoftwareSerial &serial, String at_command, String expected_answer, unsigned int timeout) {
  serial.flush();
  serial.println(at_command);
  String response = "";
  unsigned long start = millis();
  while (millis() - start < timeout) {
    while (serial.available()) {
      char c = (char)serial.read();
      response += c;
      if (response.indexOf(expected_answer) != -1) {
        Serial.println(response);
        return true;
      }
    }
  }
  Serial.println(response);
  return false;
}
