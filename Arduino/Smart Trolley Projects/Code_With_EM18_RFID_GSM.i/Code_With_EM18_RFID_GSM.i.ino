#include <SoftwareSerial.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);
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
  { "2700875A344B", 100.0, "Apple" },
  { "2700227A344B", 50.0, "Mango" },
  { "2700337A344B", 20.0, "Banana" }
};

int numItems = 3;

void setup() {
  Serial.begin(9600);
  gsmSerial.begin(9600);

  lcd.init();
  lcd.backlight();
  lcd.print("Smart Trolley");
  delay(2000);
  lcd.clear();

  gsmSerial.println("AT");
  delay(1000);
  gsmSerial.println("AT+CMGF=1");
  delay(1000);

  lcd.print("Scan item...");

  pinMode(6, INPUT_PULLUP);
}

void loop() {
  if (Serial.available()) {
    scannedUID = "";
    while (Serial.available()) {
      char c = Serial.read();
      scannedUID += c;
    }
    scannedUID.trim();

    Serial.println("Scanned UID: " + scannedUID);

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
  }

  if (digitalRead(6) == LOW) {
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
  gsmSerial.println("AT+CMGS=\"+91xxxxxxxxxx\"");
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
