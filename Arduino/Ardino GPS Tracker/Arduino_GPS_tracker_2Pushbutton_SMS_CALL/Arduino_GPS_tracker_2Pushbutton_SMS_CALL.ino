#include <TinyGPS.h>
#include <SoftwareSerial.h>

SoftwareSerial Gsm(2, 3);

TinyGPS gps;

char phone_no[] = "+9188305848xx";

const int buttonSmsPin = 5;
const int buttonCallPin = 6;

void setup() {
  Serial.begin(9600);
  Gsm.begin(9600);

  pinMode(buttonSmsPin, INPUT_PULLUP);
  pinMode(buttonCallPin, INPUT_PULLUP);

  delay(2000);

  Gsm.println("AT+CMGF=1");
  delay(500);
  Gsm.println("AT+CNMI=2,2,0,0,0");
  delay(500);
}

void loop() {

  while (Serial.available() > 0) {
    gps.encode(Serial.read());
  }

  if (digitalRead(buttonSmsPin) == LOW) {
    sendGpsSms();
    delay(10000);
  }

  if (digitalRead(buttonCallPin) == LOW) {
    makeCall();
    delay(150000);
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
