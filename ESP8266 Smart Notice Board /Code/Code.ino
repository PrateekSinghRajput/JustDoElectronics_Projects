#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);
AsyncWebServer server(80);

const char* ssid = "justDo";
const char* password = "pratik123";

const char* PARAM_INPUT_1 = "input1";

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html>
<head>
  <title>Smart Notice Board</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    body { font-family: Arial, sans-serif; background-color: #222; color: #0f0; text-align: center; }
    h1 { font-size: 2.5em; margin-top: 0.5em; margin-bottom: 0.5em; }
    #notice { 
      width: 80%; 
      margin: 1em auto;
      padding: 1em; 
      background: black;
      border: 3px solid #0f0;
      font-size: 1.5em; 
      font-weight: bold;
      white-space: nowrap;
      overflow: hidden;
      box-sizing: border-box;
    }
    #notice span {
      display: inline-block;
      padding-left: 100%;
      animation: scroll-left 15s linear infinite;
    }
    @keyframes scroll-left {
      0% { transform: translateX(0%); }
      100% { transform: translateX(-100%); }
    }
    form {
      margin-top: 1em;
    }
    input[type=text] {
      font-size: 1.2em;
      padding: 0.3em;
      width: 60%;
      max-width: 500px;
      margin-right: 0.5em;
      border: 2px solid #0f0;
      background: black;
      color: #0f0;
    }
    input[type=submit] {
      font-size: 1.2em;
      padding: 0.3em 1em;
      border: 2px solid #0f0;
      background: #0f0;
      color: black;
      cursor: pointer;
    }
    input[type=submit]:hover {
      background: #080;
    }
  </style>
</head>
<body>
  <h1>Smart Notice Board</h1>
  <div id="notice"><span>No message set</span></div>
  <form action="/get" method="GET" onsubmit="submitForm(event)">
    <input type="text" name="input1" id="input1" placeholder="Enter text to display" required>
    <input type="submit" value="Send">
  </form>

  <script>
    async function submitForm(event) {
      event.preventDefault();
      const input = document.getElementById('input1').value;
      if (!input) return;
      const response = await fetch('/get?input1=' + encodeURIComponent(input));
      if (response.ok) {
        // Update the scrolling notice text dynamically
        const notice = document.getElementById('notice').firstElementChild;
        notice.textContent = input;
        document.getElementById('input1').value = '';
      }
    }
  </script>
</body>
</html>
)rawliteral";

void notFound(AsyncWebServerRequest* request) {
  request->send(404, "text/plain", "Not found");
}

void setup() {
  Serial.begin(115200);

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Smart Notice Board");

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  Serial.println("Connecting to WiFi...");
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("WiFi Failed!");
    return;
  }

  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send_P(200, "text/html", index_html);
  });

  server.on("/get", HTTP_GET, [](AsyncWebServerRequest* request) {
    String message;
    if (request->hasParam(PARAM_INPUT_1)) {
      message = request->getParam(PARAM_INPUT_1)->value();
      lcd.clear();
      lcd.setCursor(0, 0);

      if (message.length() > 16) {
        // If message longer than display width, show first 16 chars
        lcd.print(message.substring(0, 16));
      } else {
        lcd.print(message);
      }

      Serial.println("Message received: " + message);
    } else {
      message = "No message sent";
    }
    request->send(200, "text/plain", "OK");
  });

  server.onNotFound(notFound);
  server.begin();
}

void loop() {
  // No need for repeated scrolling code here as scrolling is done in the browser notice display
}
