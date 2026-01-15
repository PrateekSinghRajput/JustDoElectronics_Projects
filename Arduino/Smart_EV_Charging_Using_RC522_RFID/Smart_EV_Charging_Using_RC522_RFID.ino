#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);

#define relay 5
Servo servo;
int servoPos = 0;
#define sensorPin1 A3
#define sensorPin2 A2
int senVal1 = 0;
int senVal2 = 0;
#define RST_PIN 8
#define SS_PIN 10
int card1Balance = 1000;
int card2Balance = 500;
#define num 7
char Data[num];
byte data_count = 0;
String num1, num2, card, card2;
int a, b;
char Key;
bool recharge = true;
MFRC522 mfrc522(SS_PIN, RST_PIN);
int state = 0;

void setup() {

  lcd.init();
  lcd.backlight();
  Serial.begin(9600);
  servo.attach(9);
  servo.write(30);
  pinMode(sensorPin1, INPUT);
  pinMode(sensorPin2, INPUT);
  pinMode(relay, OUTPUT);
  digitalWrite(relay, LOW);
  SPI.begin();
  mfrc522.PCD_Init();
  lcd.setCursor(0, 0);
  lcd.print(" Automatic EV");
  lcd.setCursor(0, 1);
  lcd.print("Charging System");
  delay(3000);
  lcd.clear();

}

void loop() {

  if (recharge == 0) {

  } else {
    lcd.setCursor(0, 0);
    lcd.print("Smart EV Charging ");
    lcd.setCursor(4, 1);
    lcd.print("   !! ");
    sensorRead();
    rfid();
    if (senVal1 == 0) {
      servoDown();
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Vehicle detected");
      delay(1000);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Put your card to");
      lcd.setCursor(0, 1);
      lcd.print("the reader......");
      delay(2000);
      lcd.clear();
    } else if (senVal2 == 0) {
      servoUp();
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Put Your Card");
      lcd.setCursor(0, 1);
      lcd.print("To Reader");
      delay(1000);
      lcd.clear();
      state = 0;
    }
  }
}

void servoDown() {
  servo.attach(9);
  for (servoPos = 30; servoPos <= 120; servoPos += 1) {
    servo.write(servoPos);
    delay(5);
  }
}
void servoUp() {
  servo.attach(9);
  for (servoPos = 120; servoPos >= 30; servoPos -= 1) {
    servo.write(servoPos);
    delay(5);
  }
}
void sensorRead() {
  senVal1 = digitalRead(sensorPin1);
  senVal2 = digitalRead(sensorPin2);
}
void rfid() {
  if (!mfrc522.PICC_IsNewCardPresent()) {
    return;
  }
  if (!mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  String content = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  content.toUpperCase();
  if (content.substring(1) == "B3 1A A3 D9") {
    if (card1Balance >= 100) {
      lcdPrint();
      card1Balance = card1Balance - 100;
      lcd.setCursor(9, 1);
      lcd.print(card1Balance);
      delay(2000);
      lcd.clear();
      state = 1;
      digitalWrite(relay, HIGH);
    } else {
      card = content.substring(1);
      LcdPrint();
      lcd.setCursor(9, 1);
      lcd.print(card1Balance);
      lcd.print(" Tk");

      delay(2000);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Please Recharge");
      delay(1000);
      lcd.clear();
      state = 0;
    }
  } else if (content.substring(1) == "F4 C2 37 BB") {
    if (card2Balance >= 100) {
      lcdPrint();
      card2Balance = card2Balance - 100;
      lcd.setCursor(9, 1);
      lcd.print(card2Balance);
      delay(2000);
      lcd.clear();
      state = 1;
    } else {
      card = content.substring(1);
      LcdPrint();
      lcd.setCursor(9, 1);
      lcd.print(card2Balance);
      lcd.print(" Tk");
      delay(2000);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Please Recharge");
      lcd.clear();
      delay(1000);
      state = 0;
    }
  }

  else {
   
    lcd.setCursor(0, 0);
    lcd.print("Unknown Vehicle");
    lcd.setCursor(0, 1);
    lcd.print("Access denied");
    delay(1500);
    lcd.clear();
    
  }
}

void clearData() {
  while (data_count != 0) {
    Data[data_count--] = 0;
  }
  return;
}


void lcdPrint() {
  delay(100);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("  Successfully");
  lcd.setCursor(0, 1);
  lcd.print(" Paid Your Bill");
  delay(1500);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Your Remaining");
  lcd.setCursor(0, 1);
  lcd.print("Balance: ");
 
}

void LcdPrint() {
  delay(100);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("  Your Balance");
  lcd.setCursor(0, 1);
  lcd.print(" is insufficent");
  delay(1500);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Your Remaining");
  lcd.setCursor(0, 1);
  lcd.print("balance: ");
}
