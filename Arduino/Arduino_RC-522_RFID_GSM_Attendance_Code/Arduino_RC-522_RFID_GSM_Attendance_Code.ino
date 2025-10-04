#include <SPI.h>
#include <SoftwareSerial.h>
#include <LiquidCrystal_I2C.h>
#include <RTClib.h>
#include <MFRC522.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);
SoftwareSerial sim(2, 3);
RTC_DS3231 rtc;

#define RST_PIN 9
#define SS_PIN 10
MFRC522 rfid(SS_PIN, RST_PIN);

#define buzzerPin 5
#define yellow 6
#define green 7

struct User {
  String rfidTag;
  String name;
  String rollNumber;
  String mobileNumber;
  int state;
  int lastActionDay;
  int lastActionMonth;
};

User users[] = {
  { "A5810B05", "Prateek", "001", "+9199756174xx", 0, 0, 0 },
  { "336A0A05", "Sonu", "002", "+9199756174xx", 0, 0, 0 },
  { "A64E3202", "Punit", "003", "+9199756174xx", 0, 0, 0 },
  { "29F427B4", "Rahul", "004", "+9199756174xx", 0, 0, 0 },
  { "735EC01C", "Ram", "005", "+9199756174xx", 0, 0, 0 }
};

void displayDateTime();
void rfidScan();
void handleUserAccess(User &user);
void handleUnauthorizedAccess();
void handleDeniedAccess(String reason);
void sendSMS(User &user, String status);
void smsSend();
void beepON();
void info();
String getCurrentDateTime();

void setup() {

  lcd.init();
  lcd.backlight();
  Serial.begin(9600);
  sim.begin(9600);

  SPI.begin();
  rfid.PCD_Init();

  if (!rtc.begin()) {
    lcd.setCursor(0, 0);
    lcd.print("RTC NOT FOUND!");
    Serial.println("Error: RTC NOT FOUND!");
    while (1)
      ;
  }

  // rtc.adjust(DateTime(2025, 10, 4, 13, 39, 0));

  pinMode(buzzerPin, OUTPUT);
  pinMode(yellow, OUTPUT);
  pinMode(green, OUTPUT);

  Serial.println("System Initialized.");

  lcd.setCursor(0, 0);
  lcd.print("    WELCOME    ");
  lcd.setCursor(0, 1);
  lcd.print("TO OUR PROJECT");
  delay(4000);
  lcd.clear();
}

void loop() {
  displayDateTime();
  rfidScan();
}

void displayDateTime() {
  DateTime now = rtc.now();

  lcd.setCursor(0, 0);
  lcd.print("Date: ");
  lcd.print(now.day());
  lcd.print("/");
  lcd.print(now.month());
  lcd.print("/");
  lcd.print(now.year());

  lcd.setCursor(0, 1);
  lcd.print("Time: ");
  if (now.hour() < 10) lcd.print('0');
  lcd.print(now.hour());
  lcd.print(":");
  if (now.minute() < 10) lcd.print('0');
  lcd.print(now.minute());
  lcd.print(":");
  if (now.second() < 10) lcd.print('0');
  lcd.print(now.second());

  delay(100);
}

void rfidScan() {
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) {
    return;
  }

  String rfidTag = "";
  for (byte i = 0; i < rfid.uid.size; i++) {
    if (rfid.uid.uidByte[i] < 0x10) {
      rfidTag += "0";
    }
    rfidTag += String(rfid.uid.uidByte[i], HEX);
  }
  rfidTag.toUpperCase();

  Serial.print("SCANNED TAG: ");
  Serial.println(rfidTag);

  int numUsers = sizeof(users) / sizeof(users[0]);
  for (int i = 0; i < numUsers; i++) {
    if (rfidTag == users[i].rfidTag) {
      handleUserAccess(users[i]);
      rfid.PICC_HaltA();
      return;
    }
  }

  handleUnauthorizedAccess();
  rfid.PICC_HaltA();
}

void handleUserAccess(User &user) {
  DateTime now = rtc.now();
  int currentDay = now.day();
  int currentMonth = now.month();

  bool alreadyActedToday = (currentDay == user.lastActionDay && currentMonth == user.lastActionMonth);

  beepON();

  if (user.state == 0) {

    if (alreadyActedToday) {
      handleDeniedAccess(user.name + ": Already Signed Out Today");
      return;
    }

    digitalWrite(green, HIGH);
    lcd.clear();
    lcd.setCursor(7, 0);
    lcd.print(user.name);
    lcd.setCursor(11, 1);
    lcd.print(user.rollNumber);
    info();
    sendSMS(user, "is Present at School (Check-In)");

    user.state = 1;
    user.lastActionDay = currentDay;
    user.lastActionMonth = currentMonth;
    Serial.print(user.name);
    Serial.println(" checked IN. New state: PRESENT.");

  } else {

    if (!alreadyActedToday) {
      handleDeniedAccess(user.name + ": Out/In mismatch or stale data");
      return;
    }

    digitalWrite(yellow, HIGH);
    lcd.clear();
    lcd.setCursor(7, 0);
    lcd.print(user.name);
    lcd.setCursor(11, 1);
    lcd.print(user.rollNumber);
    info();
    sendSMS(user, "is Out Of The School (Check-Out)");

    user.state = 0;
    user.lastActionDay = currentDay;
    user.lastActionMonth = currentMonth;
    Serial.print(user.name);
    Serial.println(" checked OUT. New state: ABSENT.");
  }

  delay(2000);
  digitalWrite(green, LOW);
  digitalWrite(yellow, LOW);
}

void handleUnauthorizedAccess() {
  handleDeniedAccess("ID : Unknown Card");
}


void handleDeniedAccess(String reason) {
  digitalWrite(buzzerPin, HIGH);
  lcd.clear();
  lcd.setCursor(0, 0);

  if (reason.indexOf("Unknown") != -1) {
    lcd.print("ACCESS DENIED");
    lcd.setCursor(0, 1);
    lcd.print("ID: Unknown Card");
  } else if (reason.indexOf("Signed Out") != -1) {
    lcd.print(reason.substring(0, 16));
    lcd.setCursor(0, 1);
    lcd.print("Signed Out Today");
  } else if (reason.indexOf("mismatch") != -1) {
    lcd.print(reason.substring(0, 16));
    lcd.setCursor(0, 1);
    lcd.print("Missing Check-In");
  }

  Serial.println("ACCESS DENIED: " + reason);
  delay(1500);
  digitalWrite(buzzerPin, LOW);
  lcd.clear();
}

void sendSMS(User &user, String status) {
  lcd.setCursor(0, 0);
  lcd.print("SMS Sending");
  Serial.print("Sending SMS to: ");
  Serial.println(user.mobileNumber);

  sim.println("AT+CMGF=1");
  delay(1000);
  sim.print("AT+CMGS=\"");
  sim.print(user.mobileNumber);
  sim.println("\"\r");
  delay(1000);

  sim.print(user.name + " (" + user.rollNumber + ") " + status + ".");
  sim.print("\nTime: ");
  sim.println(getCurrentDateTime());
  delay(100);
  sim.println((char)26);

  smsSend();
  Serial.println("SMS command sent.");
}

void smsSend() {
  for (int x = 11; x < 16; x++) {
    lcd.setCursor(x, 0);
    lcd.print(".");
    delay(1000);
  }
  lcd.clear();
}

void beepON() {
  digitalWrite(buzzerPin, HIGH);
  delay(200);
  digitalWrite(buzzerPin, LOW);
  delay(100);
}

void info() {
  lcd.setCursor(0, 0);
  lcd.print("Name : ");
  lcd.setCursor(0, 1);
  lcd.print("Roll No : ");
  delay(1500);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Authorized Access");
  delay(1000);
  lcd.clear();
}


String getCurrentDateTime() {
  DateTime now = rtc.now();
  char buffer[30];
  sprintf(buffer, "%02d/%02d/%04d %02d:%02d:%02d",
          now.day(), now.month(), now.year(), now.hour(), now.minute(), now.second());
  return String(buffer);
}
