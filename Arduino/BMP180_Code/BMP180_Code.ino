//Prateek
//wwww.prateeks.in
//https://www.youtube.com/c/JustDoElectronics/videos

#include <Adafruit_BMP085.h>
#include <Wire.h>
#include <LiquidCrystal.h>
LiquidCrystal lcd(12, 11, 10, 9, 8, 7);//RS,EN,D4,D5,D6,D7
 
char PRESSURESHOW[4];// initializing a character of size 4 for showing the result
char TEMPARATURESHOW[4];// initializing a character of size 4 for showing the temparature result
Adafruit_BMP085 bmp;
 
void setup() {
lcd.begin(16, 2);
// Print a logo message to the LCD.
lcd.print(" BMP180 Sensor");
lcd.setCursor(0, 1);
lcd.print("Temp. & Pressure");
lcd.setCursor(0, 2);
delay (3000);
 
lcd.clear();//clear display
Serial.begin(9600);
if (!bmp.begin())
{
Serial.println("ERROR");///if there is an error in communication
while (1) {}
}
}
void loop()
{
 
lcd.print("Pressure= "); // print name
String PRESSUREVALUE = String(bmp.readPressure());
// convert the reading to a char array
PRESSUREVALUE.toCharArray(PRESSURESHOW, 4);
lcd.print(PRESSURESHOW);
lcd.print("hPa ");
lcd.setCursor(0, 1);
lcd.print("Temparature=");// print name
 
String TEMPARATUREVALUE = String(bmp.readTemperature());
// convert the reading to a char array
TEMPARATUREVALUE.toCharArray(TEMPARATURESHOW, 4);
lcd.print(TEMPARATURESHOW);
lcd.print("C ");
lcd.setCursor(0, 0);//set the cursor to column 0, line1
delay(500);
}
