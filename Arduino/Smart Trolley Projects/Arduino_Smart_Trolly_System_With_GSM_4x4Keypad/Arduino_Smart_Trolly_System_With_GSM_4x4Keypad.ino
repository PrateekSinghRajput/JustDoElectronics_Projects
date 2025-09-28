#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

// Keypad configuration
const int ROW_NUM = 4;     // four rows
const int COLUMN_NUM = 4;  // four columns
char keys[ROW_NUM][COLUMN_NUM] = {
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' }
};
// Arduino pins connected to the rows and columns
byte pin_rows[ROW_NUM] = { A3, A2, A1, A0 };   // Keypad Pin 5 -> A0, Pin 8 -> A3
byte pin_column[COLUMN_NUM] = { 6, 7, 8, 9 };  // Keypad Pin 1 -> 2, Pin 4 -> 5

// Button Pins
const int add_button = 2;
const int remove_button = 3;
const int reset_button = 4;
const int buzzer_Pin = 5;

// MFRC522 RFID Reader Pins
#define SS_PIN 11
#define RST_PIN 10
MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance.

// Item definition structure
struct item {
  String item_name;
  String item_number;  // Item's RFID UID
  int item_price;
  // int item_quantity; // Removed as it's not used in the list initialization
};

// Maximum number of different items in the cart
const int MAX_ITEMS = 6;
// Predefined list of items and their prices/RFIDs
const item item_list[MAX_ITEMS] = {
  //Item Name       Item RFID Number   Item Price
  { "Salt", "B3 23 E4 11", 100 },
  { "Milk", "A3 51 A0 0D", 50 },
  { "Apple", "13 1F A3 0D", 20 },
  { "Rice", "E3 68 DC 12", 10 },
  // Remaining spots in the array are implicitly empty/zero-initialized
};

// Cart/Trolley data structure
String items[MAX_ITEMS];    // Stores item names in the trolley
int quantities[MAX_ITEMS];  // Stores quantity of each item
int prices[MAX_ITEMS];      // Stores total price (quantity * unit price) for each item

int itemCount = 0;  // Current number of *unique* items in the list

int bill_amount = 0;  // Total bill amount
int remove_buttonState = 0;
int add_buttonState = 0;
int reset_buttonState = 0;

// Flags to control adding or removing mode
int add_item_flag = 1;
int remove_item_flag = 0;

// Keypad object
Keypad keypad = Keypad(makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM);

// Variables for Mobile Number Entry
String phone_number;
char key;
int key_Count = 10;           // Number of digits to enter for the phone number
int reset_press = 0;          // Flag for after a reset, to re-prompt for number
String country_Code = "+91";  // Default country code
// String purchased_item; // Unused variable

// Function to add or update an item in the cart
void addItem(String itemName, int itemPrice) {
  // Check if the item is already in the list
  for (int i = 0; i < itemCount; i++) {
    if (items[i] == itemName) {
      // Item found, update quantity and total price
      quantities[i]++;                    // Increase quantity by 1
      prices[i] = prices[i] + itemPrice;  // Add the unit price
      return;                             // Exit the function once the item is updated
    }
  }

  // If the item wasn't found in the list, add it as a new item
  if (itemCount < MAX_ITEMS) {
    items[itemCount] = itemName;    // Add item name to list
    quantities[itemCount] = 1;      // Initial quantity is 1
    prices[itemCount] = itemPrice;  // Initial total price is the item's price
    itemCount++;                    // Increase the item count
  }
}

// Function to remove an item (decrease quantity or remove completely)
void removeItem(String itemName) {
  // Find the item in the list
  for (int i = 0; i < itemCount; i++) {
    if (items[i] == itemName) {
      // Item found, calculate the unit price
      int unitPrice = prices[i] / quantities[i];

      if (quantities[i] > 1) {
        // Decrease quantity by 1 and adjust the price
        quantities[i]--;
        prices[i] = prices[i] - unitPrice;
      } else {
        // If quantity is 1, remove the item completely from the list

        // Shift the remaining items to fill the gap
        for (int j = i; j < itemCount - 1; j++) {
          items[j] = items[j + 1];
          quantities[j] = quantities[j + 1];
          prices[j] = prices[j + 1];
        }
        itemCount--;  // Decrease item count
      }
      return;  // Exit the function once the item is removed
    }
  }
}

// Function to print the current item list to Serial (for SMS simulation)
void printItemList() {
  for (int i = 0; i < itemCount; i++) {
    // This is the message format sent for the SMS
    Serial.println(items[i] + " | Qty: " + quantities[i] + " | Total: " + prices[i] + " Rs");
    delay(100);  // Small delay to allow the serial buffer to clear for message
  }
}

// Function to simulate sending an SMS via AT commands
void SendMessage() {
  // Start message setup
  Serial.println("AT+CMGF=1");  // Sets the GSM Module in Text Mode
  delay(100);
  Serial.println("AT+CMGS=\"" + country_Code + phone_number + "\"\r");  // Mobile number
  delay(100);

  // Message Content
  Serial.println("--- Your Smart Trolley Bill ---");
  printItemList();  // Print all items in the list
  Serial.println("  Total bill : " + String(bill_amount) + " Rs");
  delay(100);

  Serial.println((char)26);  // ASCII code of CTRL+Z to send the SMS
  delay(1000);
}

void setup() {
  pinMode(remove_button, INPUT_PULLUP);  // Use INPUT_PULLUP for buttons connected to GND
  pinMode(reset_button, INPUT_PULLUP);
  pinMode(add_button, INPUT_PULLUP);
  pinMode(buzzer_Pin, OUTPUT);
  digitalWrite(buzzer_Pin, LOW);

  Serial.begin(115200);  // Standard serial communication for debugging and AT commands

  SPI.begin();         // Initiate SPI bus
  mfrc522.PCD_Init();  // Initiate MFRC522

  lcd.init();
  lcd.backlight();

  // Initial Screen: Welcome
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Smart Trolley");
  lcd.setCursor(0, 1);
  lcd.print("Billing System");
  delay(2000);

  // Prompt for Mobile Number
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Enter Mobile Num:");
  lcd.setCursor(0, 1);
  lcd.print(country_Code);
}

void loop() {
  // *** MOBILE NUMBER ENTRY LOOP ***
  while (key_Count > 0) {
    if (reset_press == 1) {
      reset_press = 0;
      // Re-prompt for Mobile Number after a reset
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Enter Mobile Num:");
      lcd.setCursor(0, 1);
      lcd.print(country_Code);
    }

    key = keypad.getKey();
    if (key && (key >= '0' && key <= '9'))  // Only accept number keys
    {
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

  // *** CHECK BUTTON STATES AND SET MODE ***
  // Assuming buttons are connected to activate when pulled LOW (using INPUT_PULLUP in setup)
  remove_buttonState = digitalRead(remove_button);
  add_buttonState = digitalRead(add_button);
  reset_buttonState = digitalRead(reset_button);

  if (remove_buttonState == LOW)  // Remove button pressed
  {
    add_item_flag = 0;
    remove_item_flag = 1;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("MODE: REMOVE");
    lcd.setCursor(0, 1);
    lcd.print("Scan to remove");
    delay(500);
  } else if (add_buttonState == LOW)  // Add button pressed
  {
    add_item_flag = 1;
    remove_item_flag = 0;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("MODE: ADD");
    lcd.setCursor(0, 1);
    lcd.print("Scan to add");
    delay(500);
  } else if (reset_buttonState == LOW)  // Reset/Checkout button pressed
  {
    // 1. Send Bill via "SMS"
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Bill Sent to:");
    lcd.setCursor(0, 1);
    lcd.print(phone_number);
    delay(1000);

    // Send the message (twice for reliability, as in original code)
    SendMessage();
    SendMessage();

    // 2. Reset Trolley Data
    key_Count = 10;
    phone_number = "";
    reset_press = 1;
    remove_item_flag = 0;
    bill_amount = 0;

    // Clear the items list
    for (int i = 0; i < itemCount; i++) {
      items[i] = "";
      quantities[i] = 0;
      prices[i] = 0;
    }
    itemCount = 0;  // Reset unique item count

    // Resetting screen
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("RESET COMPLETE");
    lcd.setCursor(0, 1);
    lcd.print("Welcome Back");
    delay(2000);
    return;  // Skip the rest of the loop to immediately enter the number entry state
  } else {
    // Display mode status if no button is actively pressed
    if (remove_item_flag == 0)  // Default Add mode or previously in Add mode
    {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("MODE: ADD");
      lcd.setCursor(0, 1);
      lcd.print("Scan your item");
    } else  // Previously in Remove mode
    {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("MODE: REMOVE");
      lcd.setCursor(0, 1);
      lcd.print("Scan to remove");
    }
    delay(500);  // Display for a moment
  }


  // *** RFID SCANNING LOGIC ***

  // Look for new cards
  if (!mfrc522.PICC_IsNewCardPresent()) {
    return;
  }
  // Select one of the cards
  if (!mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  // Get the RFID UID string
  String content = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    // Add space and then the HEX value
    content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  content.toUpperCase();

  // Loop through the item list to find a match
  for (int i = 0; i < sizeof(item_list) / sizeof(item_list[0]); i++) {
    // The content.substring(1) removes the leading space from the UID string
    if (content.substring(1) == item_list[i].item_number) {
      if (add_item_flag == 1)  // *** ADD ITEM LOGIC ***
      {
        addItem(item_list[i].item_name, item_list[i].item_price);
        bill_amount = bill_amount + item_list[i].item_price;

        digitalWrite(buzzer_Pin, HIGH);
        delay(100);  // Shorter buzzer pulse
        digitalWrite(buzzer_Pin, LOW);

        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("ADDED: " + item_list[i].item_name);
        lcd.setCursor(0, 1);
        lcd.print(String(item_list[i].item_price) + " Rs");
        delay(1000);                     // Display item info for a moment
        break;                           // Exit the for loop once the item is found and added
      } else if (remove_item_flag == 1)  // *** REMOVE ITEM LOGIC ***
      {
        // Check if the item can actually be removed (i.e., if it's in the cart)
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
          delay(1000);
          break;  // Exit the for loop once the item is found and removed
        } else {
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Item not in");
          lcd.setCursor(0, 1);
          lcd.print("your trolley");
          delay(1500);
          break;  // Item not in cart, stop checking other RFIDs
        }
      }
    }
  }  // End of item_list check

  // *** DISPLAY TOTAL BILL ***
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Total Billing:");
  lcd.setCursor(0, 1);
  lcd.print(String(bill_amount) + " Rs");
  delay(1000);  // Display the final bill amount before returning to the start-purchasing screen
}