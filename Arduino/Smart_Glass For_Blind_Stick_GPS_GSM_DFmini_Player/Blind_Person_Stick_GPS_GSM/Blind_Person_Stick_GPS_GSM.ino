//Prateek
//www.justdoelectronics.com

#include <SoftwareSerial.h>
#include <TinyGPS++.h>
int buttonpin = 6;
float lattitude, longitude;
float a[2];
float *p;
SoftwareSerial gpsSerial(2, 3);
SoftwareSerial gsmSerial(10, 11);
TinyGPSPlus gps;
const int trigPin1 = A1;
const int echoPin1 = A0;
long duration1;
int distance1;
const int trigPin2 = A2;
const int echoPin2 = A3;
long duration2;
int distance2;

void setup() {
  pinMode(trigPin1, OUTPUT);
  pinMode(echoPin1, INPUT);
  pinMode(5, OUTPUT);
  pinMode(A4, INPUT_PULLUP);
  Serial.begin(9600);
  pinMode(trigPin2, OUTPUT);
  pinMode(echoPin2, INPUT);
  pinMode(buttonpin, INPUT_PULLUP);
  Serial.begin(9600);
  delay(1000);
  gpsSerial.begin(9600);
  delay(1000);
  gsmSerial.begin(9600);
  delay(1000);
  Serial.print("—Tracking–");
  Serial.print("***Location***");
  gsmSerial.println("AT+CNMI=2,2,0,0,0");
  delay(3000);
  Serial.print("Initializing……");
  delay(2000);
  Serial.print("System Ready ");
  delay(1000);
}

void loop() {
  digitalWrite(trigPin1, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin1, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin1, LOW);
  duration1 = pulseIn(echoPin1, HIGH);
  distance1 = duration1 * 0.034 / 2;
  Serial.print("Distance1: ");
  Serial.println(distance1);
  digitalWrite(trigPin2, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin2, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin2, LOW);
  duration2 = pulseIn(echoPin2, HIGH);
  distance2 = duration2 * 0.034 / 2;
  Serial.print("Distance2: ");
  Serial.println(distance2);

  if (distance1 <= 20 || distance2 <= 20) {
    digitalWrite(5, HIGH);
    delay(1000);
    digitalWrite(5, LOW);
    digitalWrite(5, HIGH);
    delay(1000);
    digitalWrite(5, LOW);
    digitalWrite(5, HIGH);
    delay(1000);
    digitalWrite(5, LOW);
  } else if (distance1 <= 15 || distance2 <= 15) {
    digitalWrite(5, HIGH);
    delay(500);
    digitalWrite(5, LOW);
    digitalWrite(5, HIGH);
    delay(500);
    digitalWrite(5, LOW);
    digitalWrite(5, HIGH);
    delay(500);
    digitalWrite(5, LOW);
  } else
    digitalWrite(5, LOW);
  int sensorValue = digitalRead(A4);
  if (sensorValue == 1) {
    digitalWrite(5, HIGH);
    Serial.println("Rain Alert");
    delay(1500);
    digitalWrite(5, LOW);
    digitalWrite(5, HIGH);
    delay(1500);
  } else
    digitalWrite(5, LOW);
  if (digitalRead(buttonpin) == LOW) {
    Serial.println("button pressed");
    delay(2000);
    SendMessage();
  }
  if (gsmSerial.available() > 0)
    Serial.write(gsmSerial.read());
  while (gsmSerial.available()) {
    gsmSerial.read();
  }
  while (Serial.available()) {
    Serial.read();
  }
  get_gsm();
}
float *get_gps() {
  gpsSerial.listen();
  Serial.println("INSIDE get_gps");
  while (1) {
    while (gpsSerial.available() > 0) {
      gps.encode(gpsSerial.read());
    }
    if (gps.location.isUpdated()) {
      Serial.print("LAT=");
      Serial.println(gps.location.lat(), 6);
      Serial.print("LONG=");
      Serial.println(gps.location.lng(), 6);
      lattitude = gps.location.lat();
      longitude = gps.location.lng();
      break;
    }
  }
  a[0] = lattitude;
  a[1] = longitude;
  return a;
}

void get_gsm() {
  gsmSerial.listen();
  while (gsmSerial.available() > 0) {
    Serial.println("INSIDE gsmSerial.available");
    if (gsmSerial.find("Track")) {
      Serial.println("INSIDE track");
      gsmSerial.println("AT+CMGF=1");
      delay(1000);
      gsmSerial.println("AT+CMGS=\"+9188305848xx\"\r");
      delay(1000);
      p = get_gps();
      gsmSerial.listen();
      Serial.print("Your Car Location: ");
      gsmSerial.print("Your Car Location: ");
      Serial.print("LATTITUDE=");
      Serial.print(*p, 6);
      gsmSerial.print("LATTITUDE=");
      gsmSerial.print(*p, 6);
      gsmSerial.print(",");
      Serial.print("LONGITUDE=");
      Serial.print(*(p + 1), 6);
      gsmSerial.print("LONGITUDE=");
      gsmSerial.print(*(p + 1), 6);
      delay(100);
      gsmSerial.println((char)26);
      delay(1000);
    }
  }
}
void SendMessage() {
  gsmSerial.println("AT+CMGF=1");
  delay(1000);
  gsmSerial.println("AT+CMGS=\"+9188305848xx\"\r");
  delay(1000);
  gsmSerial.println("I Am In Problem Plz Help Me");
  delay(1000);
  p = get_gps();
  gsmSerial.listen();
  Serial.print("Your Position is : ");
  gsmSerial.print("position is : ");
  Serial.print("LATTITUDE=");
  Serial.print(*p, 6);
  gsmSerial.print("LATTITUDE=");
  gsmSerial.print(*p, 6);
  gsmSerial.print(",");
  Serial.print("LONGITUDE=");
  Serial.print(*(p + 1), 6);
  gsmSerial.print("LONGITUDE=");
  gsmSerial.print(*(p + 1), 6);
  delay(100);
  gsmSerial.println((char)26);
}