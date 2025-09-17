//To check status — STATUS
//To change destination number — SETNUM:+918830584864

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Level sensor pins
const int pinEmpty = 32;
const int pinQuarter = 33;
const int pinHalf = 25;
const int pinFull = 26;

String lastLevel = "";

// GSM pins
#define MODEM_RX 16
#define MODEM_TX 17

// This is now changeable via SMS
String senderNumber = "+918830584864";

void setup() {
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, MODEM_RX, MODEM_TX);  

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Water Level SMS");
  delay(2000);

  pinMode(pinEmpty, INPUT_PULLUP);
  pinMode(pinQuarter, INPUT_PULLUP);
  pinMode(pinHalf, INPUT_PULLUP);
  pinMode(pinFull, INPUT_PULLUP);

  // Set SMS mode to text
  Serial2.println("AT+CMGF=1");
  delay(1000);

  // Delete old SMS
  Serial2.println("AT+CMGD=1,4");
  delay(1000);
}

void loop() {
  String currentLevel = readWaterLevel();

  // If changed, send update
  if (currentLevel != lastLevel) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Level: ");
    lcd.print(currentLevel);

    sendSMS(senderNumber, "Water tank is now: " + currentLevel);
    lastLevel = currentLevel;
  }

  // Check incoming SMS
  checkIncomingSMS();

  delay(1000);
}

String readWaterLevel() {
  bool e = (digitalRead(pinEmpty) == LOW);
  bool q = (digitalRead(pinQuarter) == LOW);
  bool h = (digitalRead(pinHalf) == LOW);
  bool f = (digitalRead(pinFull) == LOW);

  if (f) return "FULL";
  else if (h) return "HALF";
  else if (q) return "QUARTER";
  else return "EMPTY";
}

void sendSMS(String number, String message) {
  Serial.println("Sending SMS to " + number + ": " + message);

  Serial2.println("AT+CMGF=1");
  delay(1000);

  Serial2.print("AT+CMGS=\"");
  Serial2.print(number);
  Serial2.println("\"");
  delay(500);

  Serial2.print(message);
  delay(100);
  Serial2.write(26);  // CTRL+Z
  delay(5000);
}

void checkIncomingSMS() {
  if (Serial2.available()) {
    String smsData = Serial2.readString();
    smsData.toUpperCase();

    if (smsData.indexOf("+CMT:") != -1) {
      Serial.println("SMS Received: " + smsData);

      // If contains STATUS
      if (smsData.indexOf("STATUS") != -1) {
        String level = readWaterLevel();
        sendSMS(senderNumber, "Current water tank level: " + level);
      }

      // If contains SETNUM command
      if (smsData.indexOf("SETNUM:") != -1) {
        int idx = smsData.indexOf("SETNUM:") + 7;
        String newNum = smsData.substring(idx);
        newNum.trim();

        // Remove any unwanted characters like CR/LF
        newNum.replace("\r", "");
        newNum.replace("\n", "");

        senderNumber = newNum;
        Serial.println("New number set to: " + senderNumber);
        sendSMS(senderNumber, "Recipient number updated successfully.");
      }
    }
  }
}
