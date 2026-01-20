#include <Wire.h>
#include <EEPROM.h>
#include <RTClib.h>
#include <LiquidCrystal_I2C.h>

// === ALARM MESSAGES ===
const char* alarmMessages[3] = {
  "ALARM 1 - GROUP A",
  "ALARM 2 - GROUP B", 
  "ALARM 3 - GROUP C"
};

// === PIN DEFINITIONS ===
#define PIN_BUZZER    13
#define PIN_SET_BTN   9
#define PIN_INC_BTN   10
#define PIN_NEXT_BTN  11
#define PIN_RED_LED   8
#define PIN_YELL_LED  7
#define PIN_GREEN_LED 6

LiquidCrystal_I2C lcd(0x27, 16, 2);
RTC_DS1307 rtc;

#define EEPROM_MAGIC   10
#define EEPROM_H1      11
#define EEPROM_M1      12
#define EEPROM_H2      13
#define EEPROM_M2      14
#define EEPROM_H3      15
#define EEPROM_M3      16

enum AppState { STATE_NORMAL, STATE_SET_TIME, STATE_ALARM_ACTIVE };
enum EditField { FIELD_HOUR, FIELD_MINUTE, FIELD_AMPM };

AppState appState = STATE_NORMAL;
EditField editField = FIELD_HOUR;

// === FIXED: Added missing alarmMinute array ===
uint8_t alarmHour12[3] = {0, 0, 0};
uint8_t alarmMinute[3] = {0, 0, 0};     // ← ADDED THIS LINE
uint8_t alarmIsPM[3] = {0, 0, 0};
uint8_t currentAlarm = 0;

// === BUTTON DEBOUNCE ===
bool lastSetBtn = HIGH, lastIncBtn = HIGH, lastNextBtn = HIGH;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 200;

// === ALARM TIMING ===
unsigned long alarmStartTime = 0;
uint8_t beepCount = 0;
const unsigned long BEEP_INTERVAL = 1000;
const unsigned long LED_DURATION = 60000;
const unsigned long BEEP_DURATION = 10000;

void setup() {
  Serial.begin(115200);
  
  pinMode(PIN_SET_BTN, INPUT_PULLUP);
  pinMode(PIN_INC_BTN, INPUT_PULLUP);
  pinMode(PIN_NEXT_BTN, INPUT_PULLUP);
  pinMode(PIN_BUZZER, OUTPUT);
  pinMode(PIN_RED_LED, OUTPUT);
  pinMode(PIN_YELL_LED, OUTPUT);
  pinMode(PIN_GREEN_LED, OUTPUT);
  
  allLedsOff();
  
  lcd.init();
  lcd.backlight();
  showSplash();
  
  Wire.begin();
  if (!rtc.begin()) {
    lcd.clear();
    lcd.print("RTC FAILED!");
    while(1);
  }
  if (!rtc.isrunning()) {
    rtc.adjust(DateTime(__DATE__, __TIME__));
  }
  
  loadAlarms();
}

void loop() {
  DateTime now = rtc.now();
  
  if (appState == STATE_ALARM_ACTIVE) {
    handleAlarmAdvanced(now);
    delay(50);
    return;
  }
  
  checkButtons(now);
  
  switch(appState) {
    case STATE_NORMAL:
      checkAlarms(now);
      displayTimeDate(now);
      break;
      
    case STATE_SET_TIME:
      handleSetTime();
      break;
  }
  
  delay(50);
}

void handleSetTime() {
  if (buttonPressed(PIN_INC_BTN)) {
    if (editField == FIELD_HOUR) {
      alarmHour12[currentAlarm]++;
      if (alarmHour12[currentAlarm] > 12) alarmHour12[currentAlarm] = 1;
    } else if (editField == FIELD_MINUTE) {
      alarmMinute[currentAlarm]++;
      if (alarmMinute[currentAlarm] >= 60) alarmMinute[currentAlarm] = 0;
    } else {  // FIELD_AMPM
      alarmIsPM[currentAlarm] = !alarmIsPM[currentAlarm];
    }
  }
  
  if (buttonPressed(PIN_SET_BTN)) {
    editField = (EditField)((editField + 1) % 3);
  }
  
  if (buttonPressed(PIN_NEXT_BTN)) {
    if (currentAlarm < 2) {
      currentAlarm++;
      editField = FIELD_HOUR;
    } else {
      saveAlarms();
      appState = STATE_NORMAL;
    }
  }
  
  displaySetTime(currentAlarm + 1);
}

void displayTimeDate(DateTime now) {
  static unsigned long lastUpdate = 0;
  if (millis() - lastUpdate < 500) return;
  lastUpdate = millis();
  
  lcd.clear();
  
  lcd.setCursor(0, 0);
  lcd.print("Time: ");
  
  uint8_t hour12 = now.hour() % 12;
  if (hour12 == 0) hour12 = 12;
  print2Digits(hour12);
  lcd.print(":");
  print2Digits(now.minute());
  
  lcd.setCursor(11, 0);
  if (now.hour() < 12) {
    lcd.print("AM");
  } else {
    lcd.print("PM");
  }
  
  lcd.setCursor(0, 1);
  lcd.print("Date:");
  print2Digits(now.day());
  lcd.print("/");
  print2Digits(now.month());
  lcd.print("/");
  lcd.print(now.year());
}

uint8_t hour12to24(uint8_t h12, bool isPM) {
  if (h12 == 12) {
    return isPM ? 12 : 0;
  }
  return isPM ? (h12 + 12) : h12;
}

void checkAlarms(DateTime now) {
  static bool alarmTriggered[3] = {false, false, false};
  
  for (uint8_t i = 0; i < 3; i++) {
    uint8_t alarmHour24 = hour12to24(alarmHour12[i], alarmIsPM[i]);
    if (now.hour() == alarmHour24 && now.minute() == alarmMinute[i] && 
        now.second() == 0 && !alarmTriggered[i]) {
      triggerAlarm(i);
      alarmTriggered[i] = true;
    }
    if (now.second() == 30) alarmTriggered[i] = false;
  }
}

void handleAlarmAdvanced(DateTime now) {
  unsigned long elapsed = millis() - alarmStartTime;
  
  if (elapsed < BEEP_DURATION) {
    if ((elapsed / BEEP_INTERVAL) != beepCount) {
      beepCount++;
      tone(PIN_BUZZER, 2000, 200);
      Serial.print("Beep #");
      Serial.println(beepCount);
    }
  }
  
  if (elapsed < LED_DURATION) {
    digitalWrite(PIN_RED_LED,   (currentAlarm == 0) ? HIGH : LOW);
    digitalWrite(PIN_YELL_LED,  (currentAlarm == 1) ? HIGH : LOW);
    digitalWrite(PIN_GREEN_LED, (currentAlarm == 2) ? HIGH : LOW);
    
    static bool blinkState = true;
    static unsigned long lastBlink = 0;
    if (millis() - lastBlink > 400) {
      blinkState = !blinkState;
      lastBlink = millis();
      
      lcd.clear();
      if (blinkState) {
        lcd.setCursor(0, 0);
        lcd.print(alarmMessages[currentAlarm]);
        lcd.setCursor(0, 1);
        lcd.print(" TAKE MEDICINE! ");
      }
    }
  } else {
    allLedsOff();
    noTone(PIN_BUZZER);
    appState = STATE_NORMAL;
    beepCount = 0;
  }
}

bool buttonPressed(int pin) {
  bool reading = digitalRead(pin);
  unsigned long nowTime = millis();
  
  if (reading == LOW && nowTime - lastDebounceTime > debounceDelay) {
    if ((pin == PIN_SET_BTN && lastSetBtn == HIGH) ||
        (pin == PIN_INC_BTN && lastIncBtn == HIGH) ||
        (pin == PIN_NEXT_BTN && lastNextBtn == HIGH)) {
      
      lastDebounceTime = nowTime;
      if (pin == PIN_SET_BTN) lastSetBtn = LOW;
      if (pin == PIN_INC_BTN) lastIncBtn = LOW;
      if (pin == PIN_NEXT_BTN) lastNextBtn = LOW;
      
      Serial.print("Button: Pin ");
      Serial.println(pin);
      return true;
    }
  }
  
  if (digitalRead(PIN_SET_BTN) == HIGH) lastSetBtn = HIGH;
  if (digitalRead(PIN_INC_BTN) == HIGH) lastIncBtn = HIGH;
  if (digitalRead(PIN_NEXT_BTN) == HIGH) lastNextBtn = HIGH;
  
  return false;
}

void checkButtons(DateTime now) {
  if (appState != STATE_SET_TIME) {
    if (buttonPressed(PIN_SET_BTN)) {
      appState = STATE_SET_TIME;
      currentAlarm = 0;
      editField = FIELD_HOUR;
    }
  }
}

void triggerAlarm(uint8_t alarmNum) {
  currentAlarm = alarmNum;
  appState = STATE_ALARM_ACTIVE;
  alarmStartTime = millis();
  beepCount = 0;
}

void displaySetTime(uint8_t alarmNum) {
  static unsigned long blinkTime = 0;
  static bool cursorVisible = true;
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Set Alarm ");
  lcd.print(alarmNum);
  
  lcd.setCursor(0, 1);
  uint8_t displayHour = alarmHour12[currentAlarm];
  if (displayHour == 0) displayHour = 12;
  print2Digits(displayHour);
  lcd.print(":");
  print2Digits(alarmMinute[currentAlarm]);
  
  lcd.setCursor(11, 1);
  lcd.print(alarmIsPM[currentAlarm] ? "PM" : "AM");
  
  if (millis() - blinkTime > 400) {
    cursorVisible = !cursorVisible;
    blinkTime = millis();
  }
  
  if (cursorVisible) {
    if (editField == FIELD_HOUR) {
      lcd.setCursor(0, 1);
      lcd.print("##:");
    } else if (editField == FIELD_MINUTE) {
      lcd.setCursor(3, 1);
      lcd.print(" :##");
    } else {
      lcd.setCursor(11, 1);
      lcd.print("***");
    }
  }
}

void saveAlarms() {
  EEPROM.write(EEPROM_MAGIC, 0xAA);
  for (uint8_t i = 0; i < 3; i++) {
    uint8_t h24 = hour12to24(alarmHour12[i], alarmIsPM[i]);
    EEPROM.write(EEPROM_H1 + i*2, h24);
    EEPROM.write(EEPROM_M1 + i*2, alarmMinute[i]);
  }
}

void loadAlarms() {
  if (EEPROM.read(EEPROM_MAGIC) == 0xAA) {
    for (uint8_t i = 0; i < 3; i++) {
      uint8_t h24 = EEPROM.read(EEPROM_H1 + i*2);
      alarmMinute[i] = EEPROM.read(EEPROM_M1 + i*2);
      
      if (h24 == 0 || h24 == 12) {
        alarmHour12[i] = 12;
        alarmIsPM[i] = (h24 == 0) ? 0 : 1;
      } else if (h24 > 12) {
        alarmHour12[i] = h24 - 12;
        alarmIsPM[i] = 1;
      } else {
        alarmHour12[i] = h24;
        alarmIsPM[i] = 0;
      }
    }
  }
}

void print2Digits(uint8_t num) {
  if (num < 10) lcd.print('0');
  lcd.print(num);
}

void allLedsOff() {
  digitalWrite(PIN_RED_LED, LOW);
  digitalWrite(PIN_YELL_LED, LOW);
  digitalWrite(PIN_GREEN_LED, LOW);
  digitalWrite(PIN_BUZZER, LOW);
  noTone(PIN_BUZZER);
}

void showSplash() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("MEDICINE");
  lcd.setCursor(1, 1);
  lcd.print("REMINDER v5.0");
  delay(2000);
}
