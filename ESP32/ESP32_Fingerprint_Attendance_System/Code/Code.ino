#include <Adafruit_GFX.h>
#include <Adafruit_Fingerprint.h>
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <U8g2lib.h>
#include <time.h>

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/U8X8_PIN_NONE);

#define ENROLL_BUTTON 12
#define UP_BUTTON 14
#define DOWN_BUTTON 27
#define RELAY_PIN 32

#define FINGERPRINT_RX 16
#define FINGERPRINT_TX 17

#define GSM_RX 5
#define GSM_TX 4

const char* ssid = "Prateek";
const char* password = "12345@#12345";
const char* GScriptId = "AKfycbw_CDp3mnGkVbdU4UB3EUbEB3SsXg-oOJ4WgamlKnN5AKMByVHItgw3sI4LDAg1GcS0HA";

HardwareSerial fingerSerial(2);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&fingerSerial);

HardwareSerial gsmSerial(1);

struct UserInfo {
  String name;
  String id;
  String mobile;
};
UserInfo users[] = {
  { "Patil", "EMP001", "+918830584864" },  
  { "Sohan", "EMP002", "+91xxxxxxxxxx" },
  { "Rohan", "EMP003", "+91xxxxxxxxxx" }
};
const int NUM_USERS = sizeof(users) / sizeof(users[0]);

struct AttendanceRecord {
  String date = "";
  String status = "";
};
AttendanceRecord attendanceRecords[200][2];

String user_name = "";
String user_id = "";
String attendance_status = "";

int currentEnrollId = 1;
bool enrollMode = false;
unsigned long lastButtonPress = 0;
const unsigned long BUTTON_DEBOUNCE_TIME = 200;

// DISPLAY
void displayMessage(String message, int yOffset) {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_t0_13_tr);
  int tw = u8g2.getStrWidth(message.c_str());
  int x = (u8g2.getDisplayWidth() - tw) / 2;
  u8g2.drawStr(x, yOffset, message.c_str());
  u8g2.sendBuffer();
}

String getCurrentDate() {
  time_t now = time(nullptr);
  struct tm* timeinfo = localtime(&now);
  char buf[9];
  sprintf(buf, "%04d%02d%02d", timeinfo->tm_year + 1900, timeinfo->tm_mon + 1, timeinfo->tm_mday);
  return String(buf);
}

String readSerial(unsigned long timeout) {
  String response = "";
  unsigned long start = millis();
  while (millis() - start < timeout) {
    while (gsmSerial.available()) {
      response += (char)gsmSerial.read();
      delay(1);
    }
  }
  return response;
}

bool sendATCommandCheck(const String& command, const String& expected, unsigned long timeout) {
  gsmSerial.println(command);
  String resp = readSerial(timeout);
  Serial.println("Response: " + resp);
  return resp.indexOf(expected) != -1;
}

bool sendSMS(const String& phoneNumber, const String& message) {
  Serial.println("Attempting to send SMS to " + phoneNumber);

  if (!sendATCommandCheck("AT+CMGF=1", "OK", 2000)) {
    Serial.println("Failed to set SMS text mode.");
    return false;
  }

  String cmd = "AT+CMGS=\"";
  cmd += phoneNumber;
  cmd += "\"";

  gsmSerial.println(cmd);

  String resp = readSerial(5000);
  if (resp.indexOf(">") == -1) {
    Serial.println("No '>' prompt received.");
    return false;
  }

  gsmSerial.print(message);
  gsmSerial.write(26);

  Serial.println("SMS sending started...");

  resp = readSerial(10000);

  if (resp.indexOf("OK") != -1) {
    Serial.println("SMS sent successfully.");
    return true;
  } else {
    Serial.println("SMS send failed or timed out.");
    return false;
  }
}

void setup() {
  Serial.begin(115200);
  gsmSerial.begin(9600, SERIAL_8N1, GSM_RX, GSM_TX);

  pinMode(ENROLL_BUTTON, INPUT_PULLUP);
  pinMode(UP_BUTTON, INPUT_PULLUP);
  pinMode(DOWN_BUTTON, INPUT_PULLUP);
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);

  u8g2.begin();
  displayMessage("Connecting WiFi...", 16);

  WiFi.begin(ssid, password);
  configTime(0, 0, "pool.ntp.org", "time.nist.gov");

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nWiFi Connected");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
  displayMessage("WiFi Connected!", 16);
  delay(2000);

  fingerSerial.begin(57600, SERIAL_8N1, FINGERPRINT_RX, FINGERPRINT_TX);
  delay(5);

  if (finger.verifyPassword()) {
    Serial.println("Fingerprint sensor found");
  } else {
    Serial.println("No fingerprint sensor :(");
    displayMessage("No sensor found!", 16);
    while (1) delay(1);
  }
  finger.getParameters();
  displayMessage("Ready!", 16);
  delay(1000);

  if (finger.capacity < 1) finger.capacity = 200;

  finger.getTemplateCount();
  Serial.print("Templates: ");
  Serial.println(finger.templateCount);
  displayMessage("Wait For Siganl " + String(finger.templateCount), 16);
  delay(1500);

  Serial.println("Initializing SIM800L...");
  delay(10000);
  if (!sendATCommandCheck("AT", "OK", 2000)) {
    Serial.println("SIM800L not responding");
  }

  while (!sendATCommandCheck("AT+CREG?", "+CREG: 0,1", 5000) && !sendATCommandCheck("AT+CREG?", "+CREG: 0,5", 5000)) {
    Serial.println("Waiting to register on network...");
    delay(2000);
  }
  Serial.println("Network registered");
  sendATCommandCheck("AT+CMGF=1", "OK", 2000);
}

void loop() {
  handleButtons();

  if (enrollMode) {
    enrollFingerprint(currentEnrollId);
  } else {
    identifyFingerprint();
  }
  delay(50);
}

void handleButtons() {
  unsigned long current = millis();
  if (current - lastButtonPress > BUTTON_DEBOUNCE_TIME) {
    if (digitalRead(ENROLL_BUTTON) == LOW) {
      lastButtonPress = current;
      enrollMode = !enrollMode;
      if (enrollMode) {
        displayMessage("Enroll Mode. ID: " + String(currentEnrollId), 16);
        Serial.println("Entered enroll mode");
      } else {
        displayMessage("Identification Mode", 16);
        Serial.println("Exited enroll mode");
      }
      delay(200);
    }
    if (enrollMode) {
      bool changed = false;
      if (digitalRead(UP_BUTTON) == LOW) {
        lastButtonPress = current;
        if (currentEnrollId < finger.capacity) {
          currentEnrollId++;
          changed = true;
        }
      }
      if (digitalRead(DOWN_BUTTON) == LOW) {
        lastButtonPress = current;
        if (currentEnrollId > 1) {
          currentEnrollId--;
          changed = true;
        }
      }
      if (changed) {
        displayMessage("Enroll Mode. ID: " + String(currentEnrollId), 16);
        Serial.println("Changed enroll ID: " + String(currentEnrollId));
        delay(200);
      }
    }
  }
}

void enrollFingerprint(uint8_t id) {
  int p = -1;
  displayMessage("Place finger", 16);
  Serial.println("Waiting for finger...");
  delay(50);
  p = finger.getImage();
  if (digitalRead(ENROLL_BUTTON) == LOW) {
    enrollMode = false;
    displayMessage("Exiting Enroll Mode", 16);
    delay(1000);
    return;
  }
  if (p != FINGERPRINT_OK) return;

  Serial.println("Image taken");
  p = finger.image2Tz(1);
  if (p != FINGERPRINT_OK) {
    Serial.println("Image messy");
    return;
  }
  displayMessage("Remove finger", 16);
  delay(2000);

  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
    delay(100);
  }
  Serial.println("Finger removed");

  displayMessage("Place again", 16);
  delay(1000);
  p = finger.getImage();

  if (digitalRead(ENROLL_BUTTON) == LOW) {
    enrollMode = false;
    displayMessage("Exiting Enroll Mode", 16);
    delay(1000);
    return;
  }
  if (p != FINGERPRINT_OK) return;

  Serial.println("Image taken");
  p = finger.image2Tz(2);
  if (p != FINGERPRINT_OK) {
    Serial.println("Image messy");
    return;
  }

  displayMessage("Creating model", 16);
  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    Serial.println("Fingerprints matched");
  } else {
    Serial.println("Fingerprints did not match");
    displayMessage("Prints not matched", 16);
    delay(2000);
    return;
  }

  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {
    displayMessage("Stored! ID: " + String(id), 16);
    Serial.println("Stored with ID " + String(id));
  } else {
    displayMessage("Store failed", 16);
    Serial.println("Store failed");
  }
  delay(2000);
  enrollMode = false;
  displayMessage("Identification Mode", 16);
}

void identifyFingerprint() {
  uint8_t p = finger.getImage();

  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      displayMessage("Waiting...", 16);
      return;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Comm error");
      return;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      return;
    default:
      Serial.println("Unknown error");
      return;
  }

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK) {
    Serial.println("Image messy");
    return;
  }

  p = finger.fingerSearch();
  if (p == FINGERPRINT_OK) {
    Serial.println("Match found");
    finger.LEDcontrol(FINGERPRINT_LED_ON, 50, FINGERPRINT_LED_BLUE);
    delay(1000);
    finger.LEDcontrol(FINGERPRINT_LED_OFF);
    Serial.print("ID #");
    Serial.print(finger.fingerID);
    Serial.print(", confidence ");
    Serial.println(finger.confidence);

    int fid = finger.fingerID;
    if (fid >= 1 && fid <= NUM_USERS) {
      UserInfo u = users[fid - 1];
      user_name = u.name;
      user_id = u.id;

      String today = getCurrentDate();

      AttendanceRecord& inRecord = attendanceRecords[fid][0];
      AttendanceRecord& outRecord = attendanceRecords[fid][1];

      if (inRecord.date != today) {
        inRecord.date = today;
        inRecord.status = "IN";
        attendance_status = "IN";

        if (!sendSMS(u.mobile, "Hello " + user_name + ", your IN attendance is recorded.")) {
          Serial.println("Failed to send IN SMS");
        }
      } else if (outRecord.date != today && inRecord.date == today) {
        outRecord.date = today;
        outRecord.status = "OUT";
        attendance_status = "OUT";

        if (!sendSMS(u.mobile, "Hello " + user_name + ", your OUT attendance is recorded.")) {
          Serial.println("Failed to send OUT SMS");
        }
      } else {
        attendance_status = "ALREADY MARKED";
        displayMessage("Already IN & OUT", 16);
        Serial.println("User already marked IN and OUT today");
        delay(3000);
        return;
      }
    } else {
      user_name = "Unknown";
      user_id = "Unknown";
      attendance_status = "UNKNOWN";
    }

    digitalWrite(RELAY_PIN, HIGH);
    delay(2000);
    digitalWrite(RELAY_PIN, LOW);

    displayMessage("Welcome, " + user_name + " " + attendance_status, 16);

    updateSheetGET();

    delay(3000);
  } else if (p == FINGERPRINT_NOTFOUND) {
    Serial.println("No match found");
    finger.LEDcontrol(FINGERPRINT_LED_ON, 50, FINGERPRINT_LED_RED);
    delay(1000);
    finger.LEDcontrol(FINGERPRINT_LED_OFF);
    displayMessage("Access Denied", 16);
    delay(2000);
  } else {
    Serial.println("Unknown error on search");
  }
}

void updateSheetGET() {
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClientSecure client;
    client.setInsecure();

    HTTPClient https;
    String url = "https://script.google.com/macros/s/" + String(GScriptId) + "/exec?name=" + user_name + "&status=" + attendance_status;

    Serial.println("GET URL:");
    Serial.println(url);

    if (https.begin(client, url)) {
      int httpCode = https.GET();

      if (httpCode > 0) {
        Serial.printf("Response code: %d\n", httpCode);
        if (httpCode == HTTP_CODE_OK) {
          String payload = https.getString();
          Serial.println("Response: " + payload);
          displayMessage("Sheet Updated!", 32);
        } else {
          Serial.println("Error: HTTP code not 200");
          displayMessage("Update Done", 32);
        }
      } else {
        Serial.printf("GET failed: %s\n", https.errorToString(httpCode).c_str());
        displayMessage("Update Failed!", 32);
      }
      https.end();
    } else {
      Serial.println("Unable to connect");
      displayMessage("Update Failed!", 32);
    }
  } else {
    Serial.println("WiFi not connected");
    displayMessage("WiFi Disconnected", 32);
  }
}