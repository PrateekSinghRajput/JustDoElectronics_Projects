#include <SoftwareSerial.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>

#define relay 4
#define buzzer 16

const byte ROWS = 4;
const byte COLS = 4;

char hexaKeys[ROWS][COLS] = {
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' }
};
byte rowPins[ROWS] = { 8, 7, 6, 5 };
byte colPins[COLS] = { 12, 11, 10, 9 };

Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);
LiquidCrystal_I2C lcd(0x27, 16, 2);
SoftwareSerial sim800l(2, 3);

// Fixed password
String correctPassword = "2468";

// OTP variables
int otp;
String otpstring = "";
int wrongPasswordAttempts = 0;
bool otpMode = false;  // switch to OTP mode after 3 wrong attempts

void setup() {
  sim800l.begin(4800);
  Serial.begin(115200);
  lcd.init();
  lcd.backlight();
  Serial.println("Welcome to SIM800L Secure Door Lock");
  sim800l.println("AT");
  updateSerial();
  pinMode(relay, OUTPUT);
  pinMode(buzzer, OUTPUT);
  digitalWrite(relay, LOW);
  digitalWrite(buzzer, LOW);
  delay(1000);

  lcd.setCursor(0, 0);
  lcd.print("Enter Password:");
}

void updateSerial() {
  delay(500);
  while (Serial.available()) {
    sim800l.write(Serial.read());
  }
  while (sim800l.available()) {
    Serial.write(sim800l.read());
  }
}

void loop() {
  if (!otpMode) {
    getPassword();
  } else {
    getOTP();
  }
}

void getPassword() {
  String enteredPass = "";
  lcd.setCursor(0, 1);
  lcd.print("                ");  // clear line
  lcd.setCursor(0, 1);

  while (enteredPass.length() < 4) {
    char key = customKeypad.getKey();
    if (key) {
      enteredPass += key;
      lcd.print("*");  // hide password
    }
  }

  Serial.print("Entered Password: ");
  Serial.println(enteredPass);

  if (enteredPass == correctPassword) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Access Granted");
    lcd.setCursor(0, 1);
    lcd.print("Door Opening...");
    digitalWrite(relay, HIGH);
    delay(5000);
    digitalWrite(relay, LOW);
    wrongPasswordAttempts = 0;  // reset
    lcd.clear();
    lcd.print("Enter Password:");
  } else {
    wrongPasswordAttempts++;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Wrong Password!");
    lcd.setCursor(0, 1);
    lcd.print("Attempts: " + String(wrongPasswordAttempts));

    for (int i = 0; i < 3; i++) {
      digitalWrite(buzzer, HIGH);
      delay(200);
      digitalWrite(buzzer, LOW);
      delay(200);
    }

    delay(1500);

    if (wrongPasswordAttempts >= 3) {
      otp = random(2000, 9999);
      otpstring = String(otp);
      SendOTP_SMS();
      otpMode = true;             // switch to OTP mode
      wrongPasswordAttempts = 0;  // reset
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Enter OTP:");
    } else {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Enter Password:");
    }
  }
}

void getOTP() {
  String enteredOTP = "";
  lcd.setCursor(0, 1);
  lcd.print("                ");
  lcd.setCursor(0, 1);

  while (enteredOTP.length() < 4) {
    char key = customKeypad.getKey();
    if (key) {
      enteredOTP += key;
      lcd.print(key);
    }
  }

  Serial.print("Entered OTP: ");
  Serial.println(enteredOTP);

  if (enteredOTP == otpstring) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Access Granted");
    lcd.setCursor(0, 1);
    lcd.print("Door Opening...");
    digitalWrite(relay, HIGH);
    delay(5000);
    digitalWrite(relay, LOW);
    otpMode = false;  // reset to password mode
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Enter Password:");
  } else {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Wrong OTP!");
    SendWrongOTP_SMS();

    for (int i = 0; i < 3; i++) {
      digitalWrite(buzzer, HIGH);
      delay(300);
      digitalWrite(buzzer, LOW);
      delay(300);
    }

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Enter OTP:");
  }
}

void SendOTP_SMS() {
  Serial.println("Sending OTP SMS...");
  sim800l.print("AT+CMGF=1\r");
  delay(200);
  sim800l.print("AT+CMGS=\"+9188305848xx\"\r");  // change phone number
  delay(500);
  sim800l.print("Your OTP is " + otpstring + ". Use it to unlock the door.");
  delay(500);
  sim800l.write(26);
  delay(1000);
  Serial.println("OTP Sent.");
}

void SendWrongOTP_SMS() {
  Serial.println("Sending Wrong OTP Alert...");
  sim800l.print("AT+CMGF=1\r");
  delay(200);
  sim800l.print("AT+CMGS=\"+9188305848xx\"\r");
  delay(500);
  sim800l.print("ALERT! Someone entered a wrong OTP at your door lock.");
  delay(500);
  sim800l.write(26);
  delay(1000);
  Serial.println("Wrong OTP Alert Sent.");
}
