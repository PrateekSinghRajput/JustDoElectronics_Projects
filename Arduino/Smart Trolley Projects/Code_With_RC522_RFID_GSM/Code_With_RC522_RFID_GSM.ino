#include <SoftwareSerial.h>
#include <LiquidCrystal.h>
#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 10
#define RST_PIN 9

MFRC522 mfrc522(SS_PIN, RST_PIN);

LiquidCrystal lcd(A0, A1, A2, A3, A4, A5);

SoftwareSerial gsmSerial(2, 3);

String scannedUID = "";
float totalPrice = 0;
int itemCount = 0;

struct Item {
  String uid;
  float price;
  String name;
};

Item items[] = {
  {"A1 B2 C3 D4", 10.0, "Tea"},
  {"B1 C2 D3 E4", 20.0, "Rice"},
  {"C1 D2 E3 F4", 30.0, "Milk"}
};

int numItems = 3;

void setup() {
  Serial.begin(9600);
  gsmSerial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();
  lcd.begin(16, 2);
  lcd.print("Smart Trolley");
  delay(2000);
  lcd.clear();
  gsmSerial.println("AT");
  delay(1000);
  gsmSerial.println("AT+CMGF=1");
  delay(1000);
  lcd.print("Scan item...");
}

void loop() {
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    scannedUID = "";
    for (byte i = 0; i < mfrc522.uid.size; i++) {
      scannedUID += String(mfrc522.uid.uidByte[i], HEX);
      if (i != mfrc522.uid.size - 1) scannedUID += " ";
    }
    scannedUID.toUpperCase();
    Serial.print("Scanned UID: ");
    Serial.println(scannedUID);
    float price = getPrice(scannedUID);
    if (price > 0) {
      totalPrice += price;
      itemCount++;
      lcd.clear();
      lcd.print("Added: ");
      lcd.print(price);
      lcd.setCursor(0, 1);
      lcd.print("Total: ");
      lcd.print(totalPrice);
      delay(2000);
      lcd.clear();
      lcd.print("Scan item...");
    } else {
      lcd.clear();
      lcd.print("Unknown item");
      delay(2000);
      lcd.clear();
      lcd.print("Scan item...");
    }
    mfrc522.PICC_HaltA();
  }
  if (digitalRead(6) == HIGH) {
    sendBillSMS();
    delay(5000);
  }
}

float getPrice(String uid) {
  for (int i = 0; i < numItems; i++) {
    if (uid == items[i].uid) {
      return items[i].price;
    }
  }
  return 0;
}

void sendBillSMS() {
  String message = "Shopping Bill: " + String(itemCount) + " items, Total: Rs. " + String(totalPrice);
  lcd.clear();
  lcd.print("Sending SMS...");
  gsmSerial.println("AT+CMGS=\"+9188305848xx\"");
  delay(1000);
  gsmSerial.print(message);
  delay(500);
  gsmSerial.write(26);
  delay(3000);
  lcd.clear();
  lcd.print("SMS Sent!");
  totalPrice = 0;
  itemCount = 0;
  delay(3000);
  lcd.clear();
  lcd.print("Scan item...");
}
