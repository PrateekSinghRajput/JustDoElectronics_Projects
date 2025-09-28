#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>

#define LCD_SDA_PIN 21
#define LCD_SCL_PIN 22
LiquidCrystal_I2C lcd(0x27, 16, 2);

#define SS_PIN 5
#define RST_PIN 27
MFRC522 mfrc522(SS_PIN, RST_PIN);

const int ROW_NUM = 4;
const int COLUMN_NUM = 4;
char keys[ROW_NUM][COLUMN_NUM] = {
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' }
};
byte pin_rows[ROW_NUM] = { 32, 33, 25, 2 };
byte pin_column[COLUMN_NUM] = { 4, 16, 17, 0 };

const int add_button = 14;
const int remove_button = 13;
const int reset_button = 26;
const int buzzer_Pin = 12;

struct item {
  String item_name;
  String item_number;
  int item_price;
};

const int MAX_ITEMS = 6;
const item item_list[MAX_ITEMS] = {
  { "Rice", "B3 23 E4 11", 100 },
  { "Milk", "A3 51 A0 0D", 50 },
  { "Tea", "13 1F A3 0D", 20 },
  { "Mango", "E3 68 DC 12", 10 },
};

String items[MAX_ITEMS];
int quantities[MAX_ITEMS];
int prices[MAX_ITEMS];

int itemCount = 0;
int bill_amount = 0;

int add_item_flag = 1;
int remove_item_flag = 0;
int reset_press = 0;

Keypad keypad = Keypad(makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM);
String phone_number;
char key;
int key_Count = 10;
String country_Code = "+91";

void addItem(String itemName, int itemPrice) {
  for (int i = 0; i < itemCount; i++) {
    if (items[i] == itemName) {
      quantities[i]++;
      prices[i] = prices[i] + itemPrice;
      return;
    }
  }

  if (itemCount < MAX_ITEMS) {
    items[itemCount] = itemName;
    quantities[itemCount] = 1;
    prices[itemCount] = itemPrice;
    itemCount++;
  }
}

void removeItem(String itemName) {
  for (int i = 0; i < itemCount; i++) {
    if (items[i] == itemName) {
      int unitPrice = prices[i] / quantities[i];

      if (quantities[i] > 1) {
        quantities[i]--;
        prices[i] = prices[i] - unitPrice;
      } else {
        for (int j = i; j < itemCount - 1; j++) {
          items[j] = items[j + 1];
          quantities[j] = quantities[j + 1];
          prices[j] = prices[j + 1];
        }
        itemCount--;
      }
      return;
    }
  }
}

void printItemList() {
  for (int i = 0; i < itemCount; i++) {
    Serial.println(items[i] + " | Qty: " + quantities[i] + " | Total: " + prices[i] + " Rs");
    delay(100);
  }
}

void SendMessage() {
  Serial.println("AT+CMGF=1");
  delay(100);
  Serial.println("AT+CMGS=\"" + country_Code + phone_number + "\"\r");
  delay(100);

  Serial.println("--- Your Smart Trolley Bill ---");
  printItemList();
  Serial.println("  Total bill : " + String(bill_amount) + " Rs");
  delay(100);

  Serial.println((char)26);
  delay(1000);
}

void setup() {
  pinMode(remove_button, INPUT_PULLUP);
  pinMode(reset_button, INPUT_PULLUP);
  pinMode(add_button, INPUT_PULLUP);

  pinMode(buzzer_Pin, OUTPUT);
  digitalWrite(buzzer_Pin, LOW);

  Serial.begin(115200);

  Wire.begin(LCD_SDA_PIN, LCD_SCL_PIN);
  lcd.init();
  lcd.backlight();

  SPI.begin();
  mfrc522.PCD_Init();

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Smart Trolley");
  lcd.setCursor(0, 1);
  lcd.print("Billing System");
  delay(2000);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Enter Mobile Num:");
  lcd.setCursor(0, 1);
  lcd.print(country_Code);
}

void loop() {
  while (key_Count > 0) {
    if (reset_press == 1) {
      reset_press = 0;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Enter Mobile Num:");
      lcd.setCursor(0, 1);
      lcd.print(country_Code);
    }

    key = keypad.getKey();
    if (key && (key >= '0' && key <= '9')) {
      phone_number = phone_number + String(key);
      key_Count--;

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Mobile Number:");
      lcd.setCursor(0, 1);
      lcd.print(country_Code + phone_number);
      delay(100);
    }
  }

  int remove_buttonState = digitalRead(remove_button);
  int add_buttonState = digitalRead(add_button);
  int reset_buttonState = digitalRead(reset_button);

  if (remove_buttonState == LOW) {
    add_item_flag = 0;
    remove_item_flag = 1;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("MODE: REMOVE");
    lcd.setCursor(0, 1);
    lcd.print("Scan to remove...");
    delay(500);
  } else if (add_buttonState == LOW) {
    add_item_flag = 1;
    remove_item_flag = 0;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("MODE: ADD");
    lcd.setCursor(0, 1);
    lcd.print("Scan to add...");
    delay(500);
  } else if (reset_buttonState == LOW) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Bill Sent to:");
    lcd.setCursor(0, 1);
    lcd.print(phone_number);
    delay(1000);

    SendMessage();

    key_Count = 10;
    phone_number = "";
    reset_press = 1;
    remove_item_flag = 0;
    bill_amount = 0;

    for (int i = 0; i < itemCount; i++) {
      items[i] = "";
      quantities[i] = 0;
      prices[i] = 0;
    }
    itemCount = 0;

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("RESET COMPLETE");
    lcd.setCursor(0, 1);
    lcd.print("Welcome Back");
    delay(2000);
    return;
  } else {
    if (add_item_flag == 1) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("MODE: ADD");
      lcd.setCursor(0, 1);
      lcd.print("Scan your item");
    } else {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("MODE: REMOVE");
      lcd.setCursor(0, 1);
      lcd.print("Scan to remove");
    }
    delay(100);
  }



  if (!mfrc522.PICC_IsNewCardPresent()) { return; }
  if (!mfrc522.PICC_ReadCardSerial()) { return; }

  String content = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  content.toUpperCase();

  for (int i = 0; i < MAX_ITEMS; i++) {
    if (content.substring(1) == item_list[i].item_number) {
      if (add_item_flag == 1) {
        addItem(item_list[i].item_name, item_list[i].item_price);
        bill_amount = bill_amount + item_list[i].item_price;

        digitalWrite(buzzer_Pin, HIGH);
        delay(100);
        digitalWrite(buzzer_Pin, LOW);

        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("ADDED: " + item_list[i].item_name);
        lcd.setCursor(0, 1);
        lcd.print(String(item_list[i].item_price) + " Rs");
        delay(1500);
        break;
      } else if (remove_item_flag == 1) {
        int foundIndex = -1;
        for (int j = 0; j < itemCount; j++) {
          if (items[j] == item_list[i].item_name) {
            foundIndex = j;
            break;
          }
        }

        if (foundIndex != -1) {
          bill_amount = bill_amount - item_list[i].item_price;
          removeItem(item_list[i].item_name);

          digitalWrite(buzzer_Pin, HIGH);
          delay(500);
          digitalWrite(buzzer_Pin, LOW);

          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("REMOVED:");
          lcd.setCursor(0, 1);
          lcd.print(item_list[i].item_name);
          delay(1500);
          break;
        } else {
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Item not in");
          lcd.setCursor(0, 1);
          lcd.print("your trolley");
          delay(1500);
          break;
        }
      }
    }
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Total Billing:");
  lcd.setCursor(0, 1);
  lcd.print(String(bill_amount) + " Rs");
  delay(1000);
}