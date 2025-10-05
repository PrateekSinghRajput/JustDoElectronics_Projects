#include <SoftwareSerial.h>
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"

SoftwareSerial mySoftwareSerial(11, 10);
DFRobotDFPlayerMini myDFPlayer;
void printDetail(uint8_t type, int value);

#define PAUSETIME 20000

#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);
SoftwareSerial sim800l(2, 3);

#define button1 8
#define button2 7
#define button3 6
#define button4 5
#define button5 4

bool button_State;
bool button_State1;
bool button_State2;
bool button_State3;
bool button_State4;

void setup() {
  mySoftwareSerial.begin(9600);
  Serial.begin(115200);
  Serial.println();
  Serial.println(F("Initializing DFPlayer..."));

  if (!myDFPlayer.begin(mySoftwareSerial)) {
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    while (true)
      ;
  }
  Serial.println(F("DFPlayer Mini online."));
  myDFPlayer.volume(25);

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("  Welcome To");
  lcd.setCursor(0, 1);
  lcd.print(" Our Projects");
  delay(2000);
  lcd.clear();
  pinMode(button1, INPUT_PULLUP);
  pinMode(button2, INPUT_PULLUP);
  pinMode(button3, INPUT_PULLUP);
  pinMode(button4, INPUT_PULLUP);
  pinMode(button5, INPUT_PULLUP);
  sim800l.begin(9600);
  Serial.begin(9600);
  delay(1000);
}

void loop() {
  button_State = digitalRead(button1);
  button_State1 = digitalRead(button2);
  button_State2 = digitalRead(button3);
  button_State3 = digitalRead(button4);
  button_State4 = digitalRead(button5);

  if (button_State == LOW) {
    Serial.println("Button pressed");
    delay(200);
    lcd.setCursor(0, 0);
    lcd.print("Give Me Food....");
    SendSMS();
    myDFPlayer.play(1);
  }

  if (button_State1 == LOW) {
    Serial.println("Button pressed");
    delay(200);
    lcd.setCursor(0, 0);
    lcd.print("Give me Drink...");
    SendSMS1();
    myDFPlayer.play(2);
  }

  if (button_State2 == LOW) {
    Serial.println("Button pressed");
    delay(200);
    lcd.setCursor(0, 0);
    lcd.print("Give Me Medicine");
    SendSMS2();
    myDFPlayer.play(3);
  }

  if (button_State3 == LOW) {
    Serial.println("Button pressed");
    delay(200);
    lcd.setCursor(0, 0);
    lcd.print("Give Me Dinner..");
    SendSMS3();
    myDFPlayer.play(4);
  }

  if (button_State4 == LOW) {
    Serial.println("Button pressed");
    lcd.setCursor(0, 0);
    lcd.print("Wheelchair Damage...");
    delay(200);
    SendSMS4();
    myDFPlayer.play(5);
  }

  if (sim800l.available()) {
    Serial.write(sim800l.read());
  }
}

void SendSMS() {
  Serial.println("Sending SMS...");
  sim800l.print("AT+CMGF=1\r");
  delay(100);
  sim800l.print("AT+CMGS=\"+9188305848xx\"\r");
  delay(500);
  sim800l.print(" Give Me Food..");
  delay(500);
  sim800l.print((char)26);
  delay(500);
  sim800l.println();
  Serial.println(" Give Me Food..");
  delay(500);
}

void SendSMS1() {
  Serial.println("Sending SMS...");
  sim800l.print("AT+CMGF=1\r");
  delay(100);
  sim800l.print("AT+CMGS=\"+9188305848xx\"\r");
  delay(500);
  sim800l.print(" Give Me Water...");
  delay(500);
  sim800l.print((char)26);
  delay(500);
  sim800l.println();
  Serial.println(" Give Me water...");
  delay(500);
}

void SendSMS2() {
  Serial.println("Sending SMS...");
  sim800l.print("AT+CMGF=1\r");
  delay(100);
  sim800l.print("AT+CMGS=\"+9188305848xx\"\r");
  delay(500);
  sim800l.print("Give Me Medicine");
  delay(500);
  sim800l.print((char)26);
  delay(500);
  sim800l.println();
  Serial.println(" Give Me Medicine");
  delay(500);
}

void SendSMS3() {
  Serial.println("Sending SMS...");
  sim800l.print("AT+CMGF=1\r");
  delay(100);
  sim800l.print("AT+CMGS=\"+9188305848xx\"\r");
  delay(500);
  sim800l.print(" Give Me Dinner..");
  delay(500);
  sim800l.print((char)26);
  delay(500);
  sim800l.println();
  Serial.println(" Give Me Dinner..");
  delay(500);
}

void SendSMS4() {
  Serial.println("Sending SMS...");
  sim800l.print("AT+CMGF=1\r");
  delay(100);
  sim800l.print("AT+CMGS=\"+9188305848xx\"\r");
  delay(500);
  sim800l.print("Plz Check Wheelchair Damage");
  delay(500);
  sim800l.print((char)26);
  delay(500);
  sim800l.println();
  Serial.println("Plz Check Whelchair Damage");
  delay(500);
}
