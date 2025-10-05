#include <ESP8266WiFi.h>

#define ssid "prateeksingh" 
#define password "kumar@12345" 

WiFiServer server(232); 

String header;
String state1 = "Off";
String state2 = "Off";
String state3 = "Off";
String state4 = "Off";
int LED1 = 5;
int LED2 = 4;
int LED3 = 0;
int LED4 = 2;

void setup() { 

Serial.begin(115200); 

pinMode(LED1, OUTPUT);
pinMode(LED2, OUTPUT);
pinMode(LED3, OUTPUT);
pinMode(LED4, OUTPUT);
digitalWrite(LED1, LOW);
digitalWrite(LED2, LOW);
digitalWrite(LED3, LOW);
digitalWrite(LED4, LOW);

Serial.println();
Serial.print("Connecting to ");
Serial.println(ssid);
WiFi.begin(ssid, password); 
while (WiFi.status() != WL_CONNECTED)
{
delay(500);
Serial.print(".");
}
Serial.println("");
Serial.println("WiFi connected");

server.begin(); 
Serial.println("Web server Initiated. Waiting for the ESP IP...");
delay(10000);

Serial.println(WiFi.localIP()); 
}

// runs over and over again
void loop() {
// Searching for new clients
WiFiClient client = server.available();

if (client) {
Serial.println("New client");
boolean blank_line = true; 
while (client.connected()) {
if (client.available()) {
char c = client.read();
header += c;

if (c == '\n' && blank_line) {
Serial.print(header);

if (header.indexOf("QUJDREU6MTIzNDU=") >= 0) { // Admin 12345
client.println("HTTP/1.1 200 OK");
client.println("Content-Type: text/html");
client.println("Connection: close");
client.println();

if (header.indexOf("GET / HTTP/1.1") >= 0) {
Serial.println("Main Web Page");
}
else if (header.indexOf("GET /LED1on HTTP/1.1") >= 0) {
Serial.println("GPIO 5 On");
state1 = "On";
digitalWrite(LED1, HIGH);
}
else if (header.indexOf("GET /LED1off HTTP/1.1") >= 0) {
Serial.println("GPIO 5 Off");
state1 = "Off";
digitalWrite(LED1, LOW);
}
else if (header.indexOf("GET /LED2on HTTP/1.1") >= 0) {
Serial.println("GPIO 4 On");
state2 = "On";
digitalWrite(LED2, HIGH);
}
else if (header.indexOf("GET /LED2off HTTP/1.1") >= 0) {
Serial.println("GPIO 4 Off");
state2 = "Off";
digitalWrite(LED2, LOW);
}
else if (header.indexOf("GET /LED3on HTTP/1.1") >= 0) {
Serial.println("GPIO 0 On");
state3 = "On";
digitalWrite(LED3, HIGH);
}
else if (header.indexOf("GET /LED3off HTTP/1.1") >= 0) {
Serial.println("GPIO 0 Off");
state3 = "Off";
digitalWrite(LED3, LOW);
}
else if (header.indexOf("GET /LED4on HTTP/1.1") >= 0) {
Serial.println("GPIO 2 On");
state4 = "On";
digitalWrite(LED4, HIGH);
}
else if (header.indexOf("GET /LED4off HTTP/1.1") >= 0) {
Serial.println("GPIO 2 Off");
state4 = "Off";
digitalWrite(LED4, LOW);
}
// Web page
client.println("<!DOCTYPE HTML>");
client.println("<html>");
client.println("<head>");
client.println("<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
client.println("<link rel=\"stylesheet\" href=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.3.4/css/bootstrap.min.css\">");
client.println("</head><div class=\"container\">");
client.println("<h1>JustDoElectronics</h1>");
client.println("<h2>Local WebServer</h2>");
client.println("<h3>LED 1: State: " + state1);
client.println("<div class=\"row\">");
client.println("<div class=\"col-md-2\"><a href=\"/LED1on\" class=\"btn btn-block btn-lg btn-primary\" role=\"button\">ON</a></div>");
client.println("<div class=\"col-md-2\"><a href=\"/LED1off\" class=\"btn btn-block btn-lg btn-info\" role=\"button\">OFF</a></div>");
client.println("</div>");
client.println("<h3>LED 2: State: " + state2);
client.println("<div class=\"row\">");
client.println("<div class=\"col-md-2\"><a href=\"/LED2on\" class=\"btn btn-block btn-lg btn-primary\" role=\"button\">ON</a></div>");
client.println("<div class=\"col-md-2\"><a href=\"/LED2off\" class=\"btn btn-block btn-lg btn-info\" role=\"button\">OFF</a></div>");
client.println("</div>");
client.println("<h3>LED 3: State: " + state3);
client.println("<div class=\"row\">");
client.println("<div class=\"col-md-2\"><a href=\"/LED3on\" class=\"btn btn-block btn-lg btn-primary\" role=\"button\">ON</a></div>");
client.println("<div class=\"col-md-2\"><a href=\"/LED3off\" class=\"btn btn-block btn-lg btn-info\" role=\"button\">OFF</a></div>");
client.println("</div>");
client.println("<h3>LED 4: State: " + state4);
client.println("<div class=\"row\">");
client.println("<div class=\"col-md-2\"><a href=\"/LED4on\" class=\"btn btn-block btn-lg btn-primary\" role=\"button\">ON</a></div>");
client.println("<div class=\"col-md-2\"><a href=\"/LED4off\" class=\"btn btn-block btn-lg btn-info\" role=\"button\">OFF</a></div>");
client.println("</div></div></html>");
}
else { 
client.println("HTTP/1.1 401 Unauthorized");
client.println("WWW-Authenticate: Basic realm=\"Secure\"");
client.println("Content-Type: text/html");
client.println();
client.println("<html>Authentication failed</html>");
}
header = "";
break;
}
if (c == '\n') { 
blank_line = true;
}
else if (c != '\r') { 
blank_line = false;
}
}
}
delay(1);
client.stop(); 
Serial.println("Client disconnected.");
}
}