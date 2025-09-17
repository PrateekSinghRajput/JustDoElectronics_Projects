#include <SoftwareSerial.h>
#include <AltSoftSerial.h>
#include <TinyGPS++.h>

const String PHONE = "+9188305848xx";

#define ignition_switch 4
#define ignition_sensor A0

#define rxPin 10
#define txPin 11
SoftwareSerial sim800(rxPin, txPin);

AltSoftSerial neogps;
TinyGPSPlus gps;

String receivedBuffer = "";
String sender_number;
String msg;
boolean ignition_status = false;
boolean tracking_status = false;
boolean reply_status = true;
boolean anti_theft = false;

unsigned long previousMillis = 0;
long interval = 60000;

void setup() {
  Serial.begin(115200);
  sim800.begin(9600);
  neogps.begin(9600);

  pinMode(ignition_switch, OUTPUT);
  pinMode(ignition_sensor, INPUT);

  sendSimCommand("AT");
  sendSimCommand("ATE1");
  sendSimCommand("AT+CPIN?");
  sendSimCommand("AT+CMGF=1");
  sendSimCommand("AT+CNMI=2,2,0,0,0");
}

void loop() {
  readSimData();
  readGpsData();
  handleCommands();

  ignition_status = getIgnitionStatus();
  if (anti_theft && ignition_status) {
    digitalWrite(ignition_switch, HIGH);
  } else if (!anti_theft) {
    digitalWrite(ignition_switch, LOW);
  }

  if (tracking_status && ignition_status) {
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;
      sendGpsToServer();
    }
  }
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
    if (sender_number.indexOf(PHONE) != -1) {
      doAction();
    }
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

void handleCommands() {
  if (msg.length() > 0) {
    Serial.print("Received command: ");
    Serial.println(msg);

    if (msg == "bike on") {
      digitalWrite(ignition_switch, HIGH);
      if (reply_status) sendSms("Bike has ON");
    } else if (msg == "bike off") {
      digitalWrite(ignition_switch, LOW);
      if (reply_status) sendSms("Bike has OFF");
    } else if (msg == "get location") {
      sendSmsGPS("Location");
    } else if (msg == "anti theft on") {
      anti_theft = true;
      if (reply_status) {
        sendSms("Anti-theft activated. The ignition is now locked.");
      }
    } else if (msg == "anti theft off") {
      anti_theft = false;
      if (reply_status) sendSms("Anti-theft has OFF");
    } else if (msg == "reply on") {
      reply_status = true;
      sendSms("Reply has ON");
    } else if (msg == "reply off") {
      reply_status = false;
    } else if (msg == "tracking on") {
      tracking_status = true;
      if (reply_status) sendSms("Live Tracking has ON");
    } else if (msg == "tracking off") {
      tracking_status = false;
      if (reply_status) sendSms("Live Tracking has OFF");
    } else if (msg == "tracking status") {
      if (tracking_status) {
        sendSms("Live Tracking is ON");
      } else {
        sendSms("Live Tracking is OFF");
      }
    }

    msg = "";
    sender_number = "";
  }
}

void sendSmsGPS(String text) {
  if (gps.location.isValid()) {
    sim800.print("AT+CMGF=1\r\n");
    sim800.print("AT+CMGS=\"" + PHONE + "\"\r\n");
    sim800.print("http://maps.google.com/maps?q=");
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

boolean getIgnitionStatus() {
  float val = 0;
  for (int i = 0; i < 10; i++) {
    val += analogRead(ignition_sensor);
  }
  val /= 10;
  return val > 90;
}

int sendGpsToServer() {
  Serial.println("GPS data ready to send to server...");
  if (gps.location.isValid()) {
    Serial.print("Sending GPS: ");
    Serial.print(gps.location.lat(), 6);
    Serial.print(",");
    Serial.println(gps.location.lng(), 6);
    return 1;
  }
  Serial.println("Invalid GPS data, not sending.");
  return 0;
}