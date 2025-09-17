#include <OneWire.h>
#include <DallasTemperature.h>
#include <LiquidCrystal.h>
#include <EEPROM.h>

LiquidCrystal lcd(12, 11, 10, 9, 8, 7);
int add_chk = 0;
int check_val = 3;
int c_temp = 0;
int c_temp_add = 1;
int f_temp = 0;
int f_temp_add = 2;
int set = A2, dec = A3, inc = A4, stsp = A5;
int numberOfDevices;
int relay = 5;
int buzzer = 6;
int val_tol = 0;
bool exit_stsp = false;
bool exit_set = false;
bool buz = true;
bool re_heat = false;

#define ONE_WIRE_BUS 4
#define TEMPERATURE_PRECISION 12
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
DeviceAddress tempDeviceAddress;
const int tol = 3;

void setup(void) {
  lcd.begin(16, 2);
  sensors.begin();
  pinMode(stsp, INPUT_PULLUP);
  pinMode(inc, INPUT_PULLUP);
  pinMode(dec, INPUT_PULLUP);
  pinMode(set, INPUT_PULLUP);
  pinMode(relay, OUTPUT);
  pinMode(buzzer, OUTPUT);
  digitalWrite(relay, LOW);
  digitalWrite(buzzer, LOW);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("  Temperature ");
  lcd.setCursor(0, 1);
  lcd.print("   Controller");
  numberOfDevices = sensors.getDeviceCount();

  if (EEPROM.read(add_chk) != check_val) {
    EEPROM.write(add_chk, check_val);
    EEPROM.write(c_temp_add, 50);
    f_temp = f_conv(50);
    EEPROM.write(f_temp_add, f_temp);
    c_temp = EEPROM.read(c_temp_add);
    f_temp = EEPROM.read(f_temp_add);
  } else {
    c_temp = EEPROM.read(c_temp_add);
    f_temp = EEPROM.read(f_temp_add);
  }
  delay(1500);
}

void loop(void) {
  lcd.setCursor(0, 0);
  lcd.print("PRESS START/SET");
  lcd.setCursor(0, 1);
  lcd.print("TEMP: ");
  lcd.print(EEPROM.read(c_temp_add));
  lcd.print("C/");
  lcd.print(EEPROM.read(f_temp_add));
  lcd.print("F");

  if (digitalRead(set) == LOW && exit_set == false) {
    exit_set = true;
    c_temp = EEPROM.read(c_temp_add);
    f_temp = EEPROM.read(f_temp_add);
    while (exit_set) {
      if (digitalRead(inc) == LOW) {
        c_temp += 1;
        if (c_temp > 110) c_temp = 0;
        f_temp = f_conv(c_temp);
        delay(50);
      }
      if (digitalRead(dec) == LOW) {
        c_temp -= 1;
        if (c_temp < 0) c_temp = 110;
        f_temp = f_conv(c_temp);
        delay(50);
      }
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("SET TEMPERATURE:");
      lcd.setCursor(0, 1);
      lcd.print("    ");
      lcd.print(c_temp);
      lcd.print("C/");
      lcd.print(f_temp);
      lcd.print("F");
      delay(150);
      if (digitalRead(set) == LOW) {
        delay(500);
        if (digitalRead(set) == LOW) {
          exit_set = false;
          if (EEPROM.read(c_temp_add) == c_temp) {
            lcd.clear();
            lcd.print("VALUE UNCHANGED!");
            delay(1500);
          } else {
            EEPROM.write(c_temp_add, c_temp);
            EEPROM.write(f_temp_add, f_temp);
            lcd.clear();
            lcd.print("  VALUE SAVED!");
            lcd.setCursor(0, 1);
            lcd.print("****************");
            delay(1500);
            lcd.clear();
          }
        }
      }
    }
  }

  if (digitalRead(stsp) == LOW && exit_stsp == false) {
    exit_stsp = true;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("SET:   ");
    lcd.print(EEPROM.read(c_temp_add));
    lcd.print("C/");
    lcd.print(EEPROM.read(f_temp_add));
    lcd.print("F");
    buzz();
    digitalWrite(relay, HIGH);
    while (exit_stsp) {
      sensors.requestTemperatures();
      for (int i = 0; i < numberOfDevices; i++) {
        if (sensors.getAddress(tempDeviceAddress, i)) {
          printTemperature(tempDeviceAddress);
        }
      }
      lcd.setCursor(0, 1);
      lcd.print("TEMP: ");
      lcd.print(c_temp);
      lcd.print("C/");
      lcd.print(f_temp);
      if (f_temp < 100) {
        lcd.print("F ");
      } else {
        lcd.print("F");
      }
      if (c_temp >= EEPROM.read(c_temp_add) && buz == true) {
        delay(5000);
        if (c_temp >= EEPROM.read(c_temp_add)) {
          digitalWrite(relay, LOW);
          buz = false;
          re_heat = true;
          for (int j = 0; j < 15; j++) {
            digitalWrite(buzzer, HIGH);
            delay(100);
            digitalWrite(buzzer, LOW);
            delay(100);
          }
        }
      }
      val_tol = EEPROM.read(c_temp_add) - tol;
      if (c_temp <= val_tol && re_heat == true) {
        buz = true;
        re_heat = false;
        digitalWrite(relay, HIGH);
      }

      if (digitalRead(stsp) == LOW && exit_stsp == true) {
        delay(1500);
        if (digitalRead(stsp) == LOW) {
          digitalWrite(relay, LOW);
          exit_stsp = false;
          lcd.clear();
          lcd.print("PROCESS STOPPED!");
          lcd.setCursor(0, 1);
          lcd.print("****************");
          buzz();
          delay(500);
          lcd.clear();
          break;
        }
      }
    }
  }
}

int f_conv(int x) {
  int temp;
  temp = x * 9;
  temp = temp / 5;
  temp = temp + 32;
  return temp;
}

void printTemperature(DeviceAddress deviceAddress) {
  c_temp = sensors.getTempC(deviceAddress);
  f_temp = f_conv(c_temp);
}

void buzz(void) {
  digitalWrite(buzzer, HIGH);
  delay(1000);
  digitalWrite(buzzer, LOW);
}
