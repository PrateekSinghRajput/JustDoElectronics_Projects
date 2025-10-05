
#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <SimpleTimer.h>
int VResistor = A0;
int vrdata = 0;
int Push_Button = D0;
int VRFlag = 0;

String textForSMS;

char auth[] = "xxxxxxxxxxxx";
char ssid[] = "prateeksingh";
char pass[] = "kumar@12345";

SimpleTimer timer;

void setup() {

  randomSeed(analogRead(0));
  Serial.begin(9600);
  pinMode(VResistor, INPUT);
  pinMode(Push_Button, INPUT_PULLUP);
  delay(5000);
  Blynk.begin(auth, ssid, pass);
  timer.setInterval(1000L, Sensors);
}

void loop() {

  timer.run();
  Blynk.run();
}

void sendSMS(String message) {
  Serial.print("AT+CMGF=1\r");
  delay(200);
  Serial.println("AT + CMGS = \"+918830584864\"");

  delay(200);
  Serial.println(message);
  delay(200);
  Serial.println((char)26);
  delay(200);
  Serial.println();
  delay(100);
  Blynk.begin(auth, ssid, pass);
}

void Sensors() {
  vrdata = analogRead(VResistor);
  Blynk.virtualWrite(V2, vrdata);

  if ((vrdata > 500) && (VRFlag == 0)) {
    textForSMS = "\nValue Exceeded!!!\n";
    textForSMS = textForSMS + " Sensor Value: ";
    textForSMS = textForSMS + vrdata;
    textForSMS = textForSMS + "\n";
    sendSMS(textForSMS);
    textForSMS = "";
    delay(200);
    VRFlag = 1;
  }

  if ((vrdata < 500) && (VRFlag == 1)) {
    textForSMS = "\nValue Got Normal\n";
    textForSMS = textForSMS + " Sensor Value: ";
    textForSMS = textForSMS + vrdata;
    textForSMS = textForSMS + "\n";

    sendSMS(textForSMS);
    textForSMS = "";
    delay(200);
    VRFlag = 0;
  }

  if (digitalRead(Push_Button) == LOW) {
    textForSMS = "\nButton is pressed.\n";
    textForSMS = textForSMS + " For Just";
    textForSMS = textForSMS + "Testing";
    textForSMS = textForSMS + "\n";

    sendSMS(textForSMS);
    textForSMS = "";
    delay(200);
  }
}