
#include <SoftwareSerial.h>
SoftwareSerial gprsSerial(2, 3);

#include <String.h>
#include <DHT.h>
#define DHTPIN A0

DHT dht(DHTPIN, DHT11);

void setup() {

  gprsSerial.begin(9600);
  Serial.begin(9600);
  dht.begin();
  delay(1000);
}

void loop() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  delay(100);
  Serial.print("Temperature = ");
  Serial.print(t);
  Serial.println(" °C");
  Serial.print("Humidity = ");
  Serial.print(h);
  Serial.println(" %");

  if (gprsSerial.available())
    Serial.write(gprsSerial.read());
  gprsSerial.println("AT");
  delay(1000);
  gprsSerial.println("AT+CPIN?");
  delay(1000);
  gprsSerial.println("AT+CREG?");
  delay(1000);
  gprsSerial.println("AT+CGATT?");
  delay(1000);
  gprsSerial.println("AT+CIPSHUT");
  delay(1000);
  gprsSerial.println("AT+CIPSTATUS");
  delay(2000);
  gprsSerial.println("AT+CIPMUX=0");
  delay(2000);
  ShowSerialData();
  gprsSerial.println("AT+CSTT=\"airtelgprs.com\"");
  delay(1000);
  ShowSerialData();
  gprsSerial.println("AT+CIICR");
  delay(3000);
  ShowSerialData();
  gprsSerial.println("AT+CIFSR");
  delay(2000);
  ShowSerialData();
  gprsSerial.println("AT+CIPSPRT=0");
  delay(3000);
  ShowSerialData();
  gprsSerial.println("AT+CIPSTART=\"TCP\",\"api.internet.com\",\"80\"");
  delay(6000);
  ShowSerialData();
  gprsSerial.println("AT+CIPSEND");
  delay(4000);
  ShowSerialData();

  String str = "GET https://api.thingspeak.com/update?api_key=xxxxxxxxxx&field1=" + String(t) + "&field2=" + String(h);
  Serial.println(str);
  gprsSerial.println(str);
  delay(4000);
  ShowSerialData();
  gprsSerial.println((char)26);
  delay(5000);
  gprsSerial.println();
  ShowSerialData();
  gprsSerial.println("AT+CIPSHUT");
  delay(100);
  ShowSerialData();
}

void ShowSerialData() {
  while (gprsSerial.available() != 0)
    Serial.write(gprsSerial.read());
  delay(5000);
}