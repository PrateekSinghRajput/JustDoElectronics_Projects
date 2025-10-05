#include <SoftwareSerial.h>
SoftwareSerial gsm(2, 3);

const int LDR = A0;
const int OP = 7;
const int start = 6;
const int LED = 5;
const int threshold = 300;

const unsigned long delayA = 1000UL;
const unsigned long delayB = delayA * 60;
const unsigned long delayC = delayB * 2;

void setup() {
  Serial.begin(9600);
  gsm.begin(9600);
  pinMode(OP, OUTPUT);
  pinMode(start, INPUT);
  pinMode(LED, OUTPUT);
}

void loop() {
  if (digitalRead(start) == HIGH) {
    digitalWrite(LED, HIGH);
    delay(delayC);
    while (true) {
      int lightValue = analogRead(LDR);
      Serial.println(lightValue);
      if (lightValue > threshold) {
        digitalWrite(OP, HIGH);
        Serial.println("Sending SMS...");
        sendSMS("+91XXXXXXXXXX", "Security Warning: Intruder detected.");
        Serial.println("Message sent.");
        delay(delayC);
        digitalWrite(OP, LOW);
      }
      delay(200);
    }
  } else {
    digitalWrite(LED, LOW);
  }
}

void sendSMS(const char *phoneNumber, const char *message) {
  gsm.println("AT+CMGF=1");
  delay(1000);
  gsm.print("AT+CMGS=\"");
  gsm.print(phoneNumber);
  gsm.println("\"");
  delay(1000);
  gsm.println(message);
  delay(500);
  gsm.write(26);
  delay(1000);
}
