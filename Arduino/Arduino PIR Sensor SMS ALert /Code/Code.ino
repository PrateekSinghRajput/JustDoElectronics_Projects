//Prateek
//wwww.justdoelectronics.com
//https://www.youtube.com/c/JustDoElectronics/videos

#include <SoftwareSerial.h>
SoftwareSerial SIM900(2, 3);
String textForSMS;

int pirsensor = 9;
int buzzer = 11;
int red = 7;
int green = 8;

void setup() {
  randomSeed(analogRead(0));
  Serial.begin(9600);
  SIM900.begin(9600);
  Serial.println(" logging time completed!");
  pinMode(pirsensor, INPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(red, OUTPUT);
  pinMode(green, OUTPUT);
  digitalWrite(buzzer, LOW);
  digitalWrite(red, LOW);
  digitalWrite(green, LOW);

  delay(100);
}

void loop() {


  if (digitalRead(pirsensor) == HIGH)  //
  {
    textForSMS = "\Any Person in your Room  Plz Check It ";

    digitalWrite(buzzer, HIGH);
    digitalWrite(red, HIGH);
    digitalWrite(green, LOW);
    sendSMS(textForSMS);
    Serial.println(textForSMS);
    Serial.println("message sent.");
    delay(8000);
  }
  if (digitalRead(pirsensor) == LOW)  //
  {
    digitalWrite(buzzer, LOW);
    digitalWrite(red, LOW);
    digitalWrite(green, HIGH);
    delay(1000);
  }
}

void sendSMS(String message) {
  SIM900.print("AT+CMGF=1\r");
  delay(1000);
  SIM900.println("AT + CMGS = \"+9188305848xx\"");

  delay(1000);
  SIM900.println(message);

  SIM900.println((char)26);
  delay(1000);
  SIM900.println();
  // give module time to send SMS
}
