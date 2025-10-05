#include <dht.h>
#include <SoftwareSerial.h>

SoftwareSerial gsm(2, 3);
#define DHTxxPIN A1
dht DHT;
int p = A0;
int n = A2;
int th = 45;  // temperature threshold
int msgsend = 0;

unsigned long delayA = 1000L;
unsigned long delayB = delayA * 60;
unsigned long delayC = delayB * 30;

void setup() {
  Serial.begin(9600);
  gsm.begin(9600);

  pinMode(p, OUTPUT);
  pinMode(n, OUTPUT);
  digitalWrite(p, HIGH);
  digitalWrite(n, LOW);

  sendTestSMS();
}

void loop() {
  int chk = DHT.read11(DHTxxPIN);

  if (chk == DHTLIB_ERROR_CONNECT) {
    sendNoDataSMS();
  } else {
    Serial.print("Temperature(°C) = ");
    Serial.println(DHT.temperature);
    Serial.print("Humidity(%) = ");
    Serial.println(DHT.humidity);
    Serial.println();

    if (DHT.temperature >= th) {
      sendFireAlertSMS();
    }
    delay(2000);
  }
}

void sendTestSMS() {
  gsm.println("AT+CMGF=1");
  delay(1000);
  gsm.println("AT+CMGS=\"+91xxxxxxxxxx\"\r");
  delay(1000);
  gsm.println("This is a test SMS from GSM modem");
  delay(100);
  gsm.println((char)26);
  delay(1000);
}

void sendNoDataSMS() {
  static int msgCount = 0;
  msgCount++;
  Serial.println("NO DATA");
  Serial.println("Sending SMS...");
  gsm.println("AT+CMGF=1");
  delay(1000);
  gsm.println("AT+CMGS=\"+91xxxxxxxxxx\"\r");
  delay(1000);
  gsm.println("No data from sensor/Sensor disconnected");
  delay(100);
  gsm.println((char)26);
  delay(1000);
  Serial.println("Message is sent");

  if (msgCount >= 2) {
    delay(delayC);
    msgCount = 0;
  } else {
    delay(10000);
  }
}

void sendFireAlertSMS() {
  static int msgCount = 0;
  msgCount++;
  Serial.println("Sending SMS...");
  gsm.println("AT+CMGF=1");
  delay(1000);
  gsm.println("AT+CMGS=\"+91xxxxxxxxxx\"\r");
  delay(1000);
  gsm.print("Fire Alert: ");
  gsm.print(DHT.temperature);
  gsm.println(" degree celsius");
  delay(100);
  gsm.println((char)26);
  delay(1000);
  Serial.println("Message is sent");

  if (msgCount >= 2) {
    delay(delayC);
    msgCount = 0;
  } else {
    delay(10000);
  }
}
