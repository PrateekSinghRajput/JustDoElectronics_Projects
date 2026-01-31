#include <Adafruit_GFX.h>
#include <Adafruit_Fingerprint.h>
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
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
  { "Sohan", "EMP002", "+918830584864" },
  { "Rohan", "EMP003", "+918830584864" }
};
const int NUM_USERS = sizeof(users) / sizeof(users[0]);

// Updated AttendanceRecord struct with timestamp
struct AttendanceRecord {
  String date;
  String timestamp;  // Added for exact time
  String status;
};

AttendanceRecord attendanceRecords[200][2];

String user_name = "";
String user_id = "";
String attendance_status = "";

int currentEnrollId = 1;
bool enrollMode = false;
unsigned long lastButtonPress = 0;
const unsigned long BUTTON_DEBOUNCE_TIME = 200;

bool gsmAvailable = false;

WebServer server(80);

// Helper function for current date (YYYYMMDD)
String getCurrentDate() {
  time_t now = time(nullptr);
  struct tm* timeinfo = localtime(&now);
  char buf[9];
  sprintf(buf, "%04d%02d%02d", timeinfo->tm_year + 1900, timeinfo->tm_mon + 1, timeinfo->tm_mday);
  return String(buf);
}

// NEW: Helper function for full date-time (YYYY-MM-DD HH:MM:SS)
String getCurrentDateTime() {
  time_t now = time(nullptr);
  struct tm* timeinfo = localtime(&now);
  char buf[20];
  sprintf(buf, "%04d-%02d-%02d %02d:%02d:%02d", 
          timeinfo->tm_year + 1900, timeinfo->tm_mon + 1, timeinfo->tm_mday,
          timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
  return String(buf);
}

void displayMessage(String message, int yOffset) {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_t0_13_tr);
  int tw = u8g2.getStrWidth(message.c_str());
  int x = (u8g2.getDisplayWidth() - tw) / 2;
  u8g2.drawStr(x, yOffset, message.c_str());
  u8g2.sendBuffer();
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
  Serial.println("GSM Response: " + resp);
  return resp.indexOf(expected) != -1;
}

bool checkGSMStatus() {
  if (sendATCommandCheck("AT", "OK", 1000)) {
    if (sendATCommandCheck("AT+CREG?", "+CREG: 0,1", 1000) || sendATCommandCheck("AT+CREG?", "+CREG: 0,5", 1000)) {
      sendATCommandCheck("AT+CMGF=1", "OK", 1000);
      return true;
    }
  }
  return false;
}

bool sendSMS(const String& phoneNumber, const String& message) {
  if (!gsmAvailable) {
    Serial.println("GSM not available - skipping SMS");
    return false;
  }
  
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

// UPDATED: Download handler with timestamps
void handleDownload() {
  String csv = "Employee,ID,Date,Time,Status\n";
  
  String today = getCurrentDate();
  for (int i = 0; i < NUM_USERS; i++) {
    AttendanceRecord& inRecord = attendanceRecords[i+1][0];
    AttendanceRecord& outRecord = attendanceRecords[i+1][1];
    
    if (inRecord.date == today && inRecord.timestamp != "") {
      csv += users[i].name + "," + users[i].id + "," + today + "," + inRecord.timestamp + ",IN\n";
    }
    if (outRecord.date == today && outRecord.timestamp != "") {
      csv += users[i].name + "," + users[i].id + "," + today + "," + outRecord.timestamp + ",OUT\n";
    }
  }
  
  server.sendHeader("Content-Disposition", "attachment; filename=attendance_" + getCurrentDate() + ".csv");
  server.send(200, "text/csv", csv);
  Serial.println("Attendance CSV downloaded");
}

// UPDATED: Main dashboard with date/time display
void handleRoot() {
  String html = "<!DOCTYPE html><html><head>";
  html += "<meta charset='UTF-8'>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1.0'>";
  html += "<meta http-equiv='refresh' content='5'>";
  html += "<title>Fingerprint Attendance</title>";
  html += "<style>";
  html += "body{font-family:Arial,sans-serif;margin:20px;background:#f0f0f0;}";
  html += ".container{max-width:900px;margin:0 auto;background:white;padding:20px;border-radius:10px;box-shadow:0 0 10px rgba(0,0,0,0.1);}";
  html += "h1{text-align:center;color:#333;}";
  html += ".datetime{display:flex;justify-content:space-between;align-items:center;background:#e8f5e8;padding:15px;border-radius:8px;margin-bottom:20px;font-size:16px;font-weight:bold;color:#2e7d32;}";
  html += ".datetime-info{color:#555;font-size:14px;}";
  html += "table{border-collapse:collapse;width:100%;margin:20px 0;}";
  html += "th,td{border:1px solid #ddd;padding:10px;text-align:left;font-size:14px;}";
  html += "th{background-color:#4CAF50;color:white;position:sticky;top:0;}";
  html += "tr:nth-child(even){background-color:#f2f2f2;}";
  html += "tr:hover{background-color:#e3f2fd;}";
  html += ".status-complete{color:#2e7d32;font-weight:bold;}.status-inonly{color:#ff9800;font-weight:bold;}.status-pending{color:#f44336;font-weight:bold;}";
  html += ".timestamp{color:#666;font-size:12px;}";
  html += ".gsm-ok{color:green;}.gsm-no{color:red;}";
  html += ".btn{display:inline-block;padding:10px 20px;margin:5px;text-decoration:none;border-radius:5px;font-weight:bold;}";
  html += ".btn-download{background:#2196F3;color:white;}.btn-clear{background:#ff4444;color:white;}";
  html += ".refresh-info{background:#e3f2fd;padding:10px;border-radius:5px;margin:10px 0;font-size:14px;}";
  html += "</style></head><body>";
  
  html += "<div class='container'>";
  html += "<h1>🖐️ Fingerprint Attendance System</h1>";
  
  // Date & Time Display + Status
  html += "<div class='datetime'>";
  html += "<div>📅 <strong>Server Date/Time:</strong> " + getCurrentDateTime() + "</div>";
  html += "<div class='datetime-info'>";
  html += "<strong>Mode:</strong> ";
  html += (enrollMode ? "Enroll Mode (ID: " + String(currentEnrollId) + ")" : "Identification Mode");
  html += " | <strong>GSM:</strong> <span class='";
  html += (gsmAvailable ? "gsm-ok" : "gsm-no");
  html += "'>";
  html += (gsmAvailable ? "✅ Connected" : "❌ No Signal");
  html += "</span></div>";
  html += "</div>";
  
  html += "<div class='refresh-info'>";
  html += "🔄 Auto-refreshing every 5 seconds | ";
  html += "<a href='/' style='color:#2196F3;'>🔄 Refresh Now</a>";
  html += "</div>";
  
  html += "<h2>📋 Today's Attendance Records</h2>";
  
  html += "<table>";
  html += "<tr><th>Employee</th><th>ID</th><th>IN Time</th><th>OUT Time</th><th>Status</th></tr>";
  
  String today = getCurrentDate();
  for (int i = 0; i < NUM_USERS; i++) {
    AttendanceRecord& inRecord = attendanceRecords[i+1][0];
    AttendanceRecord& outRecord = attendanceRecords[i+1][1];
    
    html += "<tr>";
    html += "<td><strong>" + users[i].name + "</strong></td>";
    html += "<td>" + users[i].id + "</td>";
    html += "<td>";
    if (inRecord.date == today && inRecord.timestamp != "") {
      html += "✅ " + inRecord.timestamp;
    } else {
      html += "⏳ Pending";
    }
    html += "</td><td>";
    if (outRecord.date == today && outRecord.timestamp != "") {
      html += "✅ " + outRecord.timestamp;
    } else {
      html += "⏳ Pending";
    }
    html += "</td><td class='status ";
    if (inRecord.date == today && outRecord.date == today && inRecord.timestamp != "" && outRecord.timestamp != "") {
      html += "status-complete'>✅ COMPLETE";
    } else if (inRecord.date == today && inRecord.timestamp != "") {
      html += "status-inonly'>🟡 IN ONLY";
    } else {
      html += "status-pending'>🔴 PENDING";
    }
    html += "</td></tr>";
  }
  html += "</table>";
  
  html += "<div style='text-align:center;margin:40px 0;'>";
  html += "<a href='/download' class='btn btn-download'>📥 Download CSV</a> ";
  html += "<a href='/clear' class='btn btn-clear' onclick=\"return confirm('Clear all today\\'s data?')\">🗑️ Clear All Data</a>";
  html += "</div>";
  html += "</div></body></html>";
  
  server.send(200, "text/html", html);
}

void handleClearData() {
  for (int i = 0; i < 200; i++) {
    for (int j = 0; j < 2; j++) {
      attendanceRecords[i][j].date = "";
      attendanceRecords[i][j].timestamp = "";  // Clear timestamp too
      attendanceRecords[i][j].status = "";
    }
  }
  server.send(200, "text/plain", "All attendance data cleared!");
  Serial.println("Attendance data cleared via web interface");
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
  
  server.on("/", handleRoot);
  server.on("/clear", handleClearData);
  server.on("/download", handleDownload);
  server.begin();
  Serial.println("Web server started! Visit: http://");
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
  displayMessage("Wait For Signal " + String(finger.templateCount), 16);
  delay(1500);

  Serial.println("Checking GSM...");
  gsmAvailable = checkGSMStatus();
  if (gsmAvailable) {
    Serial.println("GSM Ready");
  } else {
    Serial.println("GSM No Signal - Continuing without SMS");
    displayMessage("GSM No Signal", 16);
    delay(2000);
  }
}

void loop() {
  server.handleClient();
  
  static unsigned long lastGSMCheck = 0;
  if (millis() - lastGSMCheck > 30000) {
    gsmAvailable = checkGSMStatus();
    lastGSMCheck = millis();
  }
  
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

// UPDATED: identifyFingerprint with timestamp storage
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

      // UPDATED: Store timestamps
      if (inRecord.date != today) {
        inRecord.date = today;
        inRecord.timestamp = getCurrentDateTime();
        inRecord.status = "IN";
        attendance_status = "IN";
        sendSMS(u.mobile, "Hello " + user_name + ", your IN attendance recorded at " + inRecord.timestamp);
      } else if (outRecord.date != today && inRecord.date == today) {
        outRecord.date = today;
        outRecord.timestamp = getCurrentDateTime();
        outRecord.status = "OUT";
        attendance_status = "OUT";
        sendSMS(u.mobile, "Hello " + user_name + ", your OUT attendance recorded at " + outRecord.timestamp);
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