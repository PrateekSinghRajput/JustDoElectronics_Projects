#include <SoftwareSerial.h>
SoftwareSerial gsm(2, 3);

const int input = A0;
const int output = 7;
const int threshold = 600;

const unsigned long delayA = 1000L;
const unsigned long delayB = delayA * 60;     // 1 minute
const unsigned long delayC = delayB * 3;      // 3 minutes
const unsigned long delayD = delayB * 30;     // 30 minutes

void setup() {
  Serial.begin(9600);
  gsm.begin(9600);

  pinMode(output, OUTPUT);
  digitalWrite(output, LOW);

  Serial.println("Sensor waiting for optimum temperature");
  delay(delayC);

  Serial.println("Sending test SMS...");
  sendSMS("+91xxxxxxxxxx", "LPG leak, test SMS");
  Serial.println("Test SMS sent.");
}

void loop() {
  int sensorValue = analogRead(input);
  Serial.println(sensorValue);

  delay(1000);

  if (sensorValue > threshold) {
    delay(5000);
    sensorValue = analogRead(input);
    if (sensorValue > threshold) {
      Serial.println("Sending SMS...");
      Serial.println(sensorValue);

      sendSMS("+91xxxxxxxxxx", "Warning: LPG gas leak detected");

      Serial.println("SMS sent.");
      digitalWrite(output, HIGH);

      delay(delayB * 2);  // Keep output high for 2 minutes

      digitalWrite(output, LOW);

      delay(delayD);      // Wait 30 minutes before next detection cycle
    }
  }
}

void sendSMS(const char *phoneNumber, const char *message) {
  gsm.println("AT+CMGF=1"); // Text mode
  delay(1000);

  gsm.print("AT+CMGS=\"");
  gsm.print(phoneNumber);
  gsm.println("\"");
  delay(1000);

  gsm.println(message);
  delay(100);

  gsm.write(26); // Ctrl+Z ASCII to send SMS
  delay(1000);
}
