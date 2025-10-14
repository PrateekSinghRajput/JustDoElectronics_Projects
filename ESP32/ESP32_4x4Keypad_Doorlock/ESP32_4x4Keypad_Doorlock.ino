#include <Keypad.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

const byte ROWS = 4;
const byte COLS = 4;

char keys[ROWS][COLS] = {
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' }
};

byte rowPins[ROWS] = { 25, 33, 32, 15 };
byte colPins[COLS] = { 12, 14, 27, 26 };

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);
LiquidCrystal_I2C lcd(0x27, 16, 2);

const int relayPin = 4;
const int buzzerPin = 13;
const int ledPin = 2;

const String password = "AB1234";
String input = "";

void setup() {
  Serial.begin(115200);

  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, LOW);

  pinMode(buzzerPin, OUTPUT);
  digitalWrite(buzzerPin, LOW);

  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  lcd.init();
  lcd.backlight();

  lcd.setCursor(0, 0);
  lcd.print("Enter password:");
  lcd.setCursor(0, 1);
  lcd.print("______");  // 6 chars
}

void loop() {
  char key = keypad.getKey();
  if (key) {
    if (key == '*') {
      input = "";
      lcd.setCursor(0, 1);
      lcd.print("                ");
      lcd.setCursor(0, 1);
      lcd.print("______");
      Serial.println("Cleared input");
    } else if (key == 'D') {
      if (input == password) {
        lcd.setCursor(0, 0);
        lcd.print("Access Granted! ");
        lcd.setCursor(0, 1);
        lcd.print("Door unlocked   ");
        Serial.println("Access Granted");
        unlockDoor();
      } else {
        lcd.setCursor(0, 0);
        lcd.print("Access Denied!  ");
        lcd.setCursor(0, 1);
        lcd.print("Try again       ");
        Serial.println("Access Denied");
        alertWrongPassword();
      }
      input = "";
      delay(1500);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Enter password:");
      lcd.setCursor(0, 1);
      lcd.print("______");
    } else {
      if (input.length() < 6 && key != '#') {
        input += key;
        lcd.setCursor(0, 1);
        lcd.print(input);
        for (int i = input.length(); i < 6; i++) {
          lcd.print("_");
        }
        Serial.print('*');
      }
    }
  }
}

void unlockDoor() {
  digitalWrite(relayPin, HIGH);
  delay(5000);
  digitalWrite(relayPin, LOW);
}

void alertWrongPassword() {
  for (int i = 0; i < 3; i++) {
    digitalWrite(buzzerPin, HIGH);
    digitalWrite(ledPin, HIGH);
    delay(200);
    digitalWrite(buzzerPin, LOW);
    digitalWrite(ledPin, LOW);
    delay(200);
  }
}
