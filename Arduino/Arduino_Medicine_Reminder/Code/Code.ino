#include <Wire.h>
#include <EEPROM.h>
#include <RTClib.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SoftwareSerial.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define buzzer 9

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

RTC_DS1307 RTC;

int temp, inc, hours1, minut, add = 11;
int next = A0;
int INC = A1;
int set_mad = A2;
int red = 10;
int yellow = 11;
int green = 12;

int HOUR, MINUT, SECOND;

void setup() {
  Serial.begin(115200);
  delay(1000);
  Wire.begin();
  RTC.begin();

  pinMode(INC, INPUT);
  pinMode(red, OUTPUT);
  pinMode(yellow, OUTPUT);
  pinMode(green, OUTPUT);
  pinMode(next, INPUT);
  pinMode(set_mad, INPUT);
  pinMode(buzzer, OUTPUT);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print(F("Medicine Reminder"));
  display.setCursor(0, 10);
  display.print(F(" Using Arduino"));
  display.display();
  delay(2000);
  display.clearDisplay();

  if (!RTC.isrunning()) {
    RTC.adjust(DateTime(__DATE__, __TIME__));
  }
}

void loop() {
  DateTime now = RTC.now();

  if (digitalRead(set_mad) == 0) {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.print(F("  Set Medicine"));
    display.setCursor(0, 10);
    display.print(F("Reminder time"));
    display.display();
    delay(2000);

    display.clearDisplay();
    display.setCursor(0, 0);
    display.print(F("Enter Time 1"));
    defualt();
    time(1);

    display.clearDisplay();
    display.setCursor(0, 0);
    display.print(F("Enter Time 2"));
    defualt();
    delay(1000);
    time(2);

    display.clearDisplay();
    display.setCursor(0, 0);
    display.print(F("Enter Time 3"));
    defualt();
    time(3);

    display.clearDisplay();
    display.setCursor(0, 0);
    display.print(F("Medicine reminder"));
    display.setCursor(0, 10);
    display.print(F("time has set"));
    display.display();
    delay(2000);
  }

  display.clearDisplay();
  display.setCursor(0, 0);
  display.print(F("Time: "));
  display.print(HOUR = now.hour());
  display.print(":");
  display.print(MINUT = now.minute());
  display.print(":");
  display.print(SECOND = now.second());

  display.setCursor(0, 15);
  display.print(F("Date: "));
  display.print(now.day());
  display.print("/");
  display.print(now.month());
  display.print("/");
  display.print(now.year());

  display.display();

  match();

  delay(200);
}

void defualt() {
  display.setCursor(0, 20);
  display.print(HOUR);
  display.print(":");
  display.print(MINUT);
  display.print(":");
  display.print(SECOND);
  display.display();
}

void time(int x) {
  int temp = 1;
  while (temp == 1) {
    if (digitalRead(INC) == 0) {
      HOUR++;
      if (HOUR == 24) {
        HOUR = 0;
      }
      while (digitalRead(INC) == 0)
        ;
    }
    display.clearDisplay();
    display.setCursor(0, 0);
    display.print(F("Enter Time "));
    display.print(x);
    display.setCursor(0, 20);
    display.print(HOUR);
    display.print(":");
    display.print(MINUT);
    display.print(":");
    display.print(SECOND);
    display.display();
    delay(100);
    if (digitalRead(next) == 0) {
      hours1 = HOUR;
      EEPROM.write(add++, hours1);
      temp = 2;
      while (digitalRead(next) == 0)
        ;
    }
  }

  while (temp == 2) {
    if (digitalRead(INC) == 0) {
      MINUT++;
      if (MINUT == 60) {
        MINUT = 0;
      }
      while (digitalRead(INC) == 0)
        ;
    }
    display.clearDisplay();
    display.setCursor(0, 0);
    display.print(F("Enter Time "));
    display.print(x);
    display.setCursor(0, 20);
    display.print(HOUR);
    display.print(":");
    display.print(MINUT);
    display.print(":");
    display.print(SECOND);
    display.display();
    delay(100);
    if (digitalRead(next) == 0) {
      minut = MINUT;
      EEPROM.write(add++, minut);
      temp = 0;
      while (digitalRead(next) == 0)
        ;
    }
  }
  delay(1000);
}

void match() {
  int tem[17];
  for (int i = 11; i < 17; i++) {
    tem[i] = EEPROM.read(i);
  }
  if (HOUR == tem[11] && MINUT == tem[12]) {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.print(F(" Take Group One  "));
    display.setCursor(0, 10);
    display.print(F("    Medicine     "));
    display.display();

    digitalWrite(red, HIGH);
    digitalWrite(yellow, LOW);
    digitalWrite(green, LOW);
    digitalWrite(buzzer, HIGH);
    delay(5000);
    digitalWrite(red, LOW);
    digitalWrite(yellow, LOW);
    digitalWrite(green, LOW);
    digitalWrite(buzzer, LOW);
  }

  if (HOUR == tem[13] && MINUT == tem[14]) {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.print(F(" Take Group Two  "));
    display.setCursor(0, 10);
    display.print(F("    Medicine     "));
    display.display();

    digitalWrite(red, LOW);
    digitalWrite(yellow, HIGH);
    digitalWrite(green, LOW);
    digitalWrite(buzzer, HIGH);
    delay(5000);
    digitalWrite(red, LOW);
    digitalWrite(yellow, LOW);
    digitalWrite(green, LOW);
    digitalWrite(buzzer, LOW);
  }

  if (HOUR == tem[15] && MINUT == tem[16]) {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.print(F("Take Group Three "));
    display.setCursor(0, 10);
    display.print(F("   Medicine    "));
    display.display();

    digitalWrite(red, LOW);
    digitalWrite(yellow, LOW);
    digitalWrite(green, HIGH);
    digitalWrite(buzzer, HIGH);
    delay(5000);
    digitalWrite(red, LOW);
    digitalWrite(yellow, LOW);
    digitalWrite(green, LOW);
    digitalWrite(buzzer, LOW);
  }
}
