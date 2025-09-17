define BLYNK_PRINT Serial
#include <ESP32Servo.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

Servo myservo;

char auth[] = "xxxxxxxxxxxxxx";
char ssid[] = "justdoelectronics";
char pass[] = "123456789";

void setup()
{
  Serial.begin(9600);
  Blynk.begin(auth,ssid,pass);
  myservo.attach(13);
  
}
Void loop()
{
  Blynk.run();
}
BLYNK_WRITE(V1)
{
  myservo.write(160);
}
BLYNK_WRITE(V2)
{
  myservo.write(0);
}
