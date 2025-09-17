
#include <TM1637Display.h>
#include <RTClib.h>

#define CLK 5
#define DIO 18

TM1637Display display = TM1637Display(CLK, DIO);
RTC_DS1307 rtc;
// RTC_DS3231 rtc; // uncomment this line and comment the above line if using DS3231 module

unsigned long time_h = 0;
unsigned long time_m = 0;
unsigned long last_m = 0;

void setup() {
  Serial.begin(9600);
  display.clear();
  display.setBrightness(7);  // set the brightness to 7 (0:dimmest, 7:brightest)

  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (true)
      ;
  }

  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
}

void loop() {
  DateTime now = rtc.now();

  time_h = now.hour();
  time_m = now.minute();

  if (time_m != last_m) {
    unsigned long time = time_h * 100 + time_m;
    display.showNumberDecEx(time, 0b11100000, false, 4, 0);

    Serial.print(time_h);
    Serial.print(":");
    Serial.println(time_m);

    last_m = time_m;
  }
}
