#include <SoftwareSerial.h>
#include <Wire.h>
#include <LiquidCrystal_PCF8574.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>

// Pin definitions
#define TRIG_PIN D6
#define ECHO_PIN D5
#define BUZZER_PIN D7

LiquidCrystal_PCF8574 lcd(0x27);

const char* ssid = "Prateek";
const char* password = "12345@#12345";

const char* phoneNumber = "+9188305848xx";

AsyncWebServer server(80);

const int MAX_HEIGHT = 20;
// maximum garbage bin height (cm)
long currentDistance = MAX_HEIGHT;
unsigned long lastAlertTime = 0;
const unsigned long alertCooldown = 60000;

void setup() {
  Serial.begin(9600);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  lcd.begin(16, 2);
  lcd.setBacklight(255);
  lcd.print("System Init...");

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  lcd.clear();
  lcd.print("WiFi Connected");
  delay(2000);
  lcd.clear();

  Serial.println("AT+CNMI=2,1,0,0,0");
  delay(500);

  server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
    String html = R"rawliteral(
      <!DOCTYPE html>
      <html>
      <head>
        <title>Smart Garbage Monitoring</title>
        <style>
          body { font-family: Arial, sans-serif; text-align: center; }
          #dustbin {
            width: 24cm;
            height: 20cm;
            border: 5px solid black;
            border-radius: 10px;
            margin: 20px auto;
            position: relative;
            background-color: #f3f3f3;
          }
          #garbage {
            position: absolute;
            bottom: 0;
            left: 0;
            width: 100%;
            transition: height 1s ease;
            background-color: green;
          }
        </style>
      </head>
      <body>
        <h1>Smart Garbage Monitoring System</h1>
        <div id="dustbin">
          <div id="garbage" style="height: 0%;"></div>
        </div>
        <p>Garbage Level: <span id="distance">0</span> cm</p>
        <script>
          function updateData() {
            fetch('/data')
            .then(response => response.json())
            .then(data => {
              const distance = data.distance;
              const maxDist = 20;
              let level = 100 - (distance / maxDist) * 100;
              level = Math.max(0, Math.min(100, level));
              let color = 'green';
              if (level >= 70) color = 'red';
              else if (level >= 30) color = 'yellow';
              const garbageDiv = document.getElementById('garbage');
              garbageDiv.style.height = level + '%';
              garbageDiv.style.backgroundColor = color;
              document.getElementById('distance').textContent = distance;
            }).catch(console.error);
          }
          setInterval(updateData, 5000);
          updateData();
        </script>
      </body>
      </html>
    )rawliteral";
    request->send(200, "text/html", html);
  });

  server.on("/data", HTTP_GET, [](AsyncWebServerRequest* request) {
    String json = "{\"distance\": " + String(currentDistance) + "}";
    request->send(200, "application/json", json);
  });

  server.begin();
  Serial.println("Server started");
}

long measureDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000);
  if (duration == 0) return MAX_HEIGHT;
  long distance = (duration * 0.0343) / 2;
  if (distance > MAX_HEIGHT) distance = MAX_HEIGHT;
  return distance;
}

void sendSMS(const char* message) {
  while (Serial.available()) Serial.read();

  Serial.println("AT");
  delay(500);
  Serial.println("AT+CMGF=1");
  delay(500);

  unsigned long startTime = millis();
  bool registered = false;
  while (millis() - startTime < 10000) {
    Serial.println("AT+CREG?");
    delay(500);
    while (Serial.available()) {
      String resp = Serial.readString();
      if (resp.indexOf("+CREG: 0,1") >= 0 || resp.indexOf("+CREG: 0,5") >= 0) {
        registered = true;
        break;
      }
    }
    if (registered) break;
  }

  if (!registered) {
    Serial.println("GSM network not registered.");
    return;
  }

  Serial.print("AT+CMGS=\"");
  Serial.print(phoneNumber);
  Serial.println("\"");
  delay(1000);

  Serial.print(message);
  delay(500);

  Serial.write(26);
  delay(5000);

  Serial.println("SMS sent or attempted.");
}

void loop() {
  currentDistance = measureDistance();

  Serial.print("Distance: ");
  Serial.print(currentDistance);
  Serial.println(" cm");

  lcd.setCursor(0, 0);
  lcd.print("Dist: ");
  lcd.print(currentDistance);
  lcd.print(" cm   ");

  unsigned long now = millis();

  if (currentDistance < 5 && (now - lastAlertTime > alertCooldown)) {
    String alertMsg = "Garbage level high! Distance: " + String(currentDistance) + " cm.";
    digitalWrite(BUZZER_PIN, HIGH);
    lcd.setCursor(0, 1);
    lcd.print("Alert! Garbage  ");
    sendSMS(alertMsg.c_str());
    lastAlertTime = now;
  }

  if (currentDistance >= 5) {
    digitalWrite(BUZZER_PIN, LOW);
    lcd.setCursor(0, 1);
    lcd.print("                ");
  }

  delay(3000);
}
