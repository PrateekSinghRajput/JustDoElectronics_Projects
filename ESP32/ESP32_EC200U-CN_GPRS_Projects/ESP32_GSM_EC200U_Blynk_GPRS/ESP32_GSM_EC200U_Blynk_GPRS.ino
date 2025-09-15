#define BLYNK_TEMPLATE_ID "TMPL3uoHnnAya"
#define BLYNK_TEMPLATE_NAME "IoT App"
#define BLYNK_AUTH_TOKEN "tSmQNm3g66K62wTjaEQ-ADJgfy74bqHt"

#define TINY_GSM_MODEM_BG96
#define SerialAT Serial1
#define SerialMon Serial

#define RXD1 16
#define TXD1 17
#define TINY_GSM_USE_GPRS true

#define SMS_TARGET "+918830584864"

unsigned int counter = 0;
int rx = -1;
String rxString;

#include <BlynkSimpleTinyGSM.h>
BlynkTimer timer;

char auth[] = BLYNK_AUTH_TOKEN;
char apn[] = "internet";
char user[] = "";
char pass[] = "";

TinyGsm modem(SerialAT);

void setup() {
  Serial.begin(115200);
  delay(100);
  SerialAT.begin(115200, SERIAL_8N1, RXD1, TXD1);
  delay(5000);

  Serial.println("Modem Reset, Please Wait");
  SerialAT.println("AT+CRESET");
  delay(1000);
  SerialAT.println("AT+CRESET");
  delay(5000);
  SerialAT.flush();

  Serial.println("Echo Off");
  SerialAT.println("ATE0");
  delay(1000);
  SerialAT.println("ATE0");
  rxString = SerialAT.readString();
  Serial.print("Got: ");
  Serial.println(rxString);
  rx = rxString.indexOf("OK");
  if (rx != -1)
    Serial.println("Modem Ready");
  delay(1000);

  Serial.println("SIM card check");
  SerialAT.println("AT+CPIN?");
  rxString = SerialAT.readString();
  Serial.print("Got: ");
  Serial.println(rxString);
  rx = rxString.indexOf("+CPIN: READY");
  if (rx != -1)
    Serial.println("SIM Card Ready");
  delay(1000);

  Serial.println("Sending initial SMS...");
  if (modem.sendSMS(SMS_TARGET, "Board is connected")) {
    Serial.println("SMS sent successfully!");
  } else {
    Serial.println("SMS failed to send.");
  }

  String modemInfo = modem.getModemInfo();
  Serial.println("Trying Blynk");
  Blynk.begin(auth, modem, apn, user, pass, "blynk.cloud", 80);
}

void loop() {
  Serial.println("Send Data on Blynk App");
  delay(1000);
  Blynk.virtualWrite(V1, counter);
  delay(2000);
  counter++;

  Blynk.run();
  timer.run();
}