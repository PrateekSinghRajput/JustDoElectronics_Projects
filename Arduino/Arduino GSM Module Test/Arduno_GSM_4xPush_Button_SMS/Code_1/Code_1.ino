#include <SoftwareSerial.h>

SoftwareSerial mySerial(2, 3);

void setup()
{
  
  Serial.begin(9600);
  
 
  mySerial.begin(9600);

  Serial.println("Initializing..."); 
  delay(1000);

  mySerial.println("AT");
  updateSerial();

  mySerial.println("AT+CMGF=1"); 
  updateSerial();
  mySerial.println("AT+CMGS=\"+918830584864\""); // enter your phone number here (prefix country code)
  updateSerial();
  mySerial.print("Hello From justdoelectronics"); // enter your message here
  updateSerial();
  mySerial.write(26);
}

void loop()
{
}

void updateSerial()
{
  delay(500);
  while (Serial.available()) 
  {
    mySerial.write(Serial.read());
  }
  while(mySerial.available()) 
  {
    Serial.write(mySerial.read());
  }
}
