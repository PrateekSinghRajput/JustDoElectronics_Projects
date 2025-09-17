#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

String scannedUID = "";
float totalPrice = 0;
int itemCount = 0;

struct Item {
  String uid;
  float price;
  String name;
};

Item items[] = {
  { "2700875A344B", 100.0, "Rice" },
  { "2700227A344B", 250.0, "Tea" },
  { "2700875A344B", 10.0, "Apple" },
  { "2700227A344B", 50.0, "Banana" },
  { "2700337A344B", 180.0, "Milk" }
};

int numItems = 3;

void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  lcd.print("Smart Trolley");
  delay(2000);
  lcd.clear();
  lcd.print("Scan item...");
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
}

float getPrice(String uid) {
  for (int i = 0; i < numItems; i++) {
    if (uid == items[i].uid) {
      return items[i].price;
    }
  }
  return 0;
}
