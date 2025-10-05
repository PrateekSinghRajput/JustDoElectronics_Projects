
int Pin1 = D1; 
int Pin2 = D2;  
int Pin3 = D5;  
int Pin4 = D6;  


int pole1[] = { 0, 0, 0, 0, 0, 1, 1, 1, 0 };  
int pole2[] = { 0, 0, 0, 1, 1, 1, 0, 0, 0 }; 
int pole3[] = { 0, 1, 1, 1, 0, 0, 0, 0, 0 };  
int pole4[] = { 1, 1, 0, 0, 0, 0, 0, 1, 0 };  //pole4, 8 step values

int poleStep = 0;
int dirStatus = 3; 

String buttonTitle1[] = { "CCW", "CW" };  
String buttonTitle2[] = { "CCW", "CW" };
String argId[] = { "ccw", "cw" };

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#ifndef STASSID
#define STASSID "prateeksingh"
#define STAPSK "kumar@12345"
#endif

const char *ssid = STASSID;
const char *password = STAPSK;

ESP8266WebServer server(80);

void handleRoot() {

  String HTML = "<!DOCTYPE html>\
  <html>\
  <head>\
  \t\n<title> Stepper Motor Control</title>\
  \t\n<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\
  \n<style>\
 \nhtml,body{\t\nwidth:100%\;\nheight:100%\;\nmargin:0}\n*{box-sizing:border-box}\n.colorAll{\n\tbackground-color:#90ee90}\n.colorBtn{\n\tbackground-color:#add8e6}\n.angleButtdon,a{\n\tfont-size:30px\;\nborder:1px solid #ccc\;\ndisplay:table-caption\;\npadding:7px 10px\;\ntext-decoration:none\;\ncursor:pointer\;\npadding:5px 6px 7px 10px}a{\n\tdisplay:block}\n.btn{\n\tmargin:5px\;\nborder:none\;\ndisplay:inline-block\;\nvertical-align:middle\;\ntext-align:center\;\nwhite-space:nowrap}\n";

  HTML += "</style>\n\n</head>\n\n<body>\n<h1> Stepper Motor Control </h1>\n";

  if (dirStatus == 2) {
    HTML += "\n\t<h2><span style=\"background-color: #FFFF00\">Motor Running in CW</span></h2>\n";
  } else if (dirStatus == 1) {
    HTML += "\n\t<h2><span style=\"background-color: #FFFF00\">Motor Running in CCW</span></h2>\n";
  } else {
    HTML += "\n\t<h2><span style=\"background-color: #FFFF00\">Motor OFF</span></h2>\n";
  }
  if (dirStatus == 1) {
    HTML += "\t<div class=\"btn\">\n\t\t<a class=\"angleButton\" style=\"background-color:#f56464\"  href=\"/motor?";
    HTML += argId[0];
    HTML += "=off\">";
    HTML += buttonTitle1[0];  //motor ON title
  } else {
    HTML += "\t<div class=\"btn\">\n\t\t<a class=\"angleButton \" style=\"background-color:#90ee90\"  href=\"/motor?";
    HTML += argId[0];
    HTML += "=on\">";
    HTML += buttonTitle2[0];  //motor OFF title
  }
  HTML += "</a>\t\n\t</div>\n\n";
  if (dirStatus == 2) {
    HTML += "\t<div class=\"btn\">\n\t\t<a class=\"angleButton\" style=\"background-color:#f56464\"  href=\"/motor?";
    HTML += argId[1];
    HTML += "=off\">";
    HTML += buttonTitle1[1];  //motor ON title
  } else {
    HTML += "\t<div class=\"btn\">\n\t\t<a class=\"angleButton \" style=\"background-color:#90ee90\"  href=\"/motor?";
    HTML += argId[1];
    HTML += "=on\">";
    HTML += buttonTitle2[1];  //motor OFF title
  }
  HTML += "</a>\t\n\t</div>\n\n";

  HTML += "\t\n</body>\n</html>\n";
  server.send(200, "text/html", HTML);
}

void handleNotFound() {

  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";

  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }

  server.send(404, "text/plain", message);
}

void setup(void) {

  pinMode(Pin1, OUTPUT);  //define pin for ULN2003 in1
  pinMode(Pin2, OUTPUT);  //define pin for ULN2003 in2
  pinMode(Pin3, OUTPUT);  //define pin for ULN2003 in3
  pinMode(Pin4, OUTPUT);  //define pin for ULN2003 in4


  Serial.begin(115200);  //initialize the serial monitor
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: http://");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/motor", HTTP_GET, motorControl);
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  server.handleClient();
  MDNS.update();
  //Watch details at my Arduino Course at Udemy.com
  if (dirStatus == 1) {
    poleStep++;
    driveStepper(poleStep);
  } else if (dirStatus == 2) {
    poleStep--;
    driveStepper(poleStep);
  } else {
    driveStepper(8);
  }
  if (poleStep > 7) {
    poleStep = 0;
  }
  if (poleStep < 0) {
    poleStep = 7;
  }

  delay(1);
}

void motorControl() {

  if (server.arg(argId[0]) == "on") {
    dirStatus = 1;  // CCW

  } else if (server.arg(argId[0]) == "off") {
    dirStatus = 3;  // motor OFF

  } else if (server.arg(argId[1]) == "on") {
    dirStatus = 2;  // CW

  } else if (server.arg(argId[1]) == "off") {
    dirStatus = 3;  // motor OFF
  }

  handleRoot();
}

void driveStepper(int c) {
  digitalWrite(Pin1, pole1[c]);
  digitalWrite(Pin2, pole2[c]);
  digitalWrite(Pin3, pole3[c]);
  digitalWrite(Pin4, pole4[c]);
}