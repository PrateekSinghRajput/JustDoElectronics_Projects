#include <DS1307.h>

uint8_t set_Sec = 0;      /* Set the Seconds */
uint8_t set_Minute = 47;  /* Set the Minutes */
uint8_t set_Hour = 9;     /* Set the Hours */
uint8_t set_Day = 2;      /* Set the Day */
uint8_t set_Month = 9;    /* Set the Month */
uint16_t set_Year = 2024; /* Set the Year */

uint8_t sec, minute, hour, day, month;
uint16_t year;
DS1307 rtc;

void setup(void) {
  Serial.begin(115200); 
  rtc.begin();
  rtc.set(set_Sec, set_Minute, set_Hour, set_Day, set_Month, set_Year);
  rtc.stop(); 

  rtc.start(); 
  delay(1000); 
  Serial.print("You have set: ");
  Serial.print("\nTime: ");
  Serial.print(set_Hour, DEC);
  Serial.print(":");
  Serial.print(set_Minute, DEC);
  Serial.print(":");
  Serial.print(set_Sec, DEC);

  Serial.print("\nDate: ");
  Serial.print(set_Day, DEC);
  Serial.print(".");
  Serial.print(set_Month, DEC);
  Serial.print(".");
  Serial.print(set_Year, DEC);
  Serial.println("");
}

void loop(void) {}