#include <SoftwareSerial.h>
SoftwareSerial sim(2, 3);
int _timeout;
String _buffer;
String number = "+918830584864";

void setup() {
  delay(7000);
  Serial.begin(9600);
  _buffer.reserve(50);
  Serial.println("Sistem Started...");
  sim.begin(9600);
  delay(1000);
  Serial.println("Type s to send an SMS, r to receive an SMS, and c to make a call");
}

void loop() {
  if (Serial.available() > 0)
    switch (Serial.read()) {
      case 's':
        SendMessage();
        break;
      case 'r':
        RecieveMessage();
        break;
      case 'c':
        callNumber();
        break;
    }
  if (sim.available() > 0)
    Serial.write(sim.read());
}

void SendMessage() {
  Serial.println("Sending Message");
  sim.println("AT+CMGF=1");
  delay(1000);
  Serial.println("Set SMS Number");
  sim.println("AT+CMGS=\"" + number + "\"\r");
  delay(1000);
  String SMS = "Hi I am Prateek";
  sim.println(SMS);
  delay(100);
  sim.println((char)26);
  delay(1000);
  _buffer = _readSerial();
}

void RecieveMessage() {
  Serial.println("SIM800L Read an SMS");
  delay(1000);
  sim.println("AT+CNMI=2,2,0,0,0");
  delay(1000);
  Serial.write("Unread Message done");
}
String _readSerial() {
  _timeout = 0;
  while (!sim.available() && _timeout < 12000) {
    delay(13);
    _timeout++;
  }
  if (sim.available()) {
    return sim.readString();
  }
}

void callNumber() {
  sim.print(F("ATD"));
  sim.print(number);
  sim.print(F(";\r\n"));
  _buffer = _readSerial();
  Serial.println(_buffer);
}