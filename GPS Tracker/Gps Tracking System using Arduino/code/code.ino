#include <SoftwareSerial.h>
#include <AltSoftSerial.h>
#include <TinyGPS++.h>

const String PHONE = "+9188305848xx";

#define rxPin 10
#define txPin 11
SoftwareSerial sim800(rxPin, txPin);

AltSoftSerial neogps;
TinyGPSPlus gps;

String receivedBuffer = "";
String sender_number;
String msg;

void setup() {
  Serial.begin(115200);
  sim800.begin(9600);
  neogps.begin(9600);

  sendSimCommand("AT");
  sendSimCommand("ATE1");
  sendSimCommand("AT+CPIN?");
  sendSimCommand("AT+CMGF=1");
  sendSimCommand("AT+CNMI=2,2,0,0,0");
}

void loop() {
  readSimData();
  readGpsData();
  handleSmsCommand();
}

void sendSimCommand(String command) {
  sim800.println(command);
}

void readSimData() {
  while (sim800.available()) {
    char c = sim800.read();
    receivedBuffer += c;
    if (c == '\n') {
      parseData(receivedBuffer);
      receivedBuffer = "";
    }
  }
}

void readGpsData() {
  while (neogps.available()) {
    gps.encode(neogps.read());
  }
}

void parseData(String buff) {
  buff.trim();
  if (buff.startsWith("+CMTI:")) {
    int commaIndex = buff.indexOf(",");
    if (commaIndex != -1) {
      String smsIndex = buff.substring(commaIndex + 1);
      smsIndex.trim();
      sendSimCommand("AT+CMGR=" + smsIndex);
    }
  } else if (buff.startsWith("+CMGR:")) {
    extractSms(buff);
  } else if (buff.startsWith("+CMGS:")) {
    Serial.println("SMS Sent Successfully.");
  }
}

void extractSms(String buff) {
  int startOfContent = buff.lastIndexOf("\n") + 1;
  int endOfContent = buff.lastIndexOf("\r\nOK");

  if (startOfContent > 0 && endOfContent > startOfContent) {
    msg = buff.substring(startOfContent, endOfContent);
    msg.trim();
    msg.toLowerCase();
  } else {
    msg = "";
  }

  int numberStart = buff.indexOf("+CMGR:") + 1;
  int numberEnd = buff.indexOf(",", numberStart);
  if (numberStart != -1 && numberEnd != -1) {
    sender_number = buff.substring(numberStart, numberEnd);
    sender_number.trim();
  } else {
    sender_number = "";
  }
}

void handleSmsCommand() {
  if (msg.length() > 0 && sender_number.indexOf(PHONE) != -1) {
    if (msg == "get location") {
      sendSmsGPS();
    }
    msg = "";
    sender_number = "";
  }
}

void sendSmsGPS() {
  if (gps.location.isValid()) {
    sim800.print("AT+CMGF=1\r\n");
    sim800.print("AT+CMGS=\"" + PHONE + "\"\r\n");
    sim800.print("http://maps.google.com/maps?q=?q=");
    sim800.print(gps.location.lat(), 6);
    sim800.print(",");
    sim800.print(gps.location.lng(), 6);
    sim800.write(0x1A);
  } else {
    sendSms("GPS signal not available. Please try again.");
  }
}

void sendSms(String text) {
  sim800.print("AT+CMGF=1\r\n");
  sim800.print("AT+CMGS=\"" + PHONE + "\"\r\n");
  sim800.print(text);
  sim800.write(0x1A);
}