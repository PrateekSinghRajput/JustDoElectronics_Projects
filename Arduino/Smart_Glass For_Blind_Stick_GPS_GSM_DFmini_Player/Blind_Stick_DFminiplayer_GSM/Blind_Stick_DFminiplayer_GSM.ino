//Prateek
//www.justdoelectronics.com

#include <Arduino.h>
#include <NewPing.h>
#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>
SoftwareSerial mySerial(2, 3);

#define MAX_DISTANCE 200

const int trigPin1 = 5;
const int echoPin1 = 4;
const int trigPin2 = 7;
const int echoPin2 = 6;
const int trigPin3 = 9;
const int echoPin3 = 8;

#define DO_PIN 12
int buttonpin = A0;

int minLeftDistance = 80;
int minCenterDistance = 80;
int minRightDistance = 80;

static unsigned long timer = millis();

int waitTime = 3500;

NewPing sonarLeft(trigPin1, echoPin1, MAX_DISTANCE);
NewPing sonarCenter(trigPin2, echoPin2, MAX_DISTANCE);
NewPing sonarRight(trigPin3, echoPin3, MAX_DISTANCE);

SoftwareSerial mp3SoftwareSerial(11, 10);  // RX, TX
DFRobotDFPlayerMini DFPlayer;

void printDetail(uint8_t type, int value) {
  switch (type) {
    case TimeOut:
      Serial.println(F("Time Out!"));
      break;
    case WrongStack:
      Serial.println(F("Stack Wrong!"));
      break;
    case DFPlayerCardInserted:
      Serial.println(F("Card Inserted!"));
      break;
    case DFPlayerCardRemoved:
      Serial.println(F("Card Removed!"));
      break;
    case DFPlayerCardOnline:
      Serial.println(F("Card Online!"));
      break;
    case DFPlayerPlayFinished:
      Serial.print(F("Number:"));
      Serial.print(value);
      Serial.println(F(" Play Finished!"));
      break;
    case DFPlayerError:
      Serial.print(F("DFPlayerError:"));
      switch (value) {
        case Busy:
          Serial.println(F("Card not found"));
          break;
        case Sleeping:
          Serial.println(F("Sleeping"));
          break;
        case SerialWrongStack:
          Serial.println(F("Get Wrong Stack"));
          break;
        case CheckSumNotMatch:
          Serial.println(F("Check Sum Not Match"));
          break;
        case FileIndexOut:
          Serial.println(F("File Index Out of Bound"));
          break;
        case FileMismatch:
          Serial.println(F("Cannot Find File"));
          break;
        case Advertise:
          Serial.println(F("In Advertise"));
          break;
        default:
          break;
      }
      break;
    default:
      break;
  }
}

void leftAlert(int distance) {

  if (millis() - timer > waitTime) {
    timer = millis();
    DFPlayer.playLargeFolder(01, distance + 1);
  }
  if (DFPlayer.available()) {
    printDetail(DFPlayer.readType(), DFPlayer.read());
  }
}

void centerAlert(int distance) {

  if (millis() - timer > waitTime) {
    timer = millis();
    DFPlayer.playLargeFolder(02, distance + 1);
  }
  if (DFPlayer.available()) {
    printDetail(DFPlayer.readType(), DFPlayer.read());
  }
}

void rightAlert(int distance) {

  if (millis() - timer > waitTime) {
    timer = millis();
    DFPlayer.playLargeFolder(03, distance + 1);
  }
  if (DFPlayer.available()) {
    printDetail(DFPlayer.readType(), DFPlayer.read());
  }
}

void setupDFPlayer() {

  mp3SoftwareSerial.begin(9600);
  Serial.println();
  Serial.println(F("Blind assistancs smart glass..."));
  Serial.println(F("Initializing DFPlayer ... (May take 3~5 seconds)"));

  if (!DFPlayer.begin(mp3SoftwareSerial)) {
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    while (true)
      ;
  }
  Serial.println(F("DFPlayer Mini online."));

  DFPlayer.setTimeOut(500);  //serial communictaion time out 500ms
  DFPlayer.volume(29);       //volume value (0~30)
  DFPlayer.EQ(DFPLAYER_EQ_NORMAL);
  DFPlayer.outputDevice(DFPLAYER_DEVICE_SD);
}

void setup() {
  Serial.begin(9600);
  mySerial.begin(9600);
  pinMode(DO_PIN, INPUT);
  pinMode(13, OUTPUT);
  pinMode(buttonpin, INPUT_PULLUP);
  setupDFPlayer();
}

void loop() {
  rain();
  button();
  delay(29);
  int leftDistance = sonarLeft.ping_cm();
  int centerDistance = sonarCenter.ping_cm();
  int rightDistance = sonarRight.ping_cm();

  // For debugging
  // Serial.print("Left: ");
  // Serial.print(leftDistance);
  // Serial.print(", Center: ");
  // Serial.print(centerDistance);
  // Serial.print(", Right: ");
  // Serial.println(rightDistance);

  if (leftDistance < minLeftDistance && leftDistance > 1) {

    Serial.print("Obstacle ");
    Serial.print(leftDistance);
    Serial.println(" CM to the left");

    leftAlert(leftDistance);

  } else {
  }

  if (centerDistance < minCenterDistance && centerDistance > 1) {

    Serial.print("Obstacle ");
    Serial.print(centerDistance);
    Serial.println(" CM ahed");

    centerAlert(centerDistance);

  } else {
  }

  if (rightDistance < minRightDistance && rightDistance > 1) {

    Serial.print("Obstacle ");
    Serial.print(rightDistance);
    Serial.println(" CM to the right");

    rightAlert(rightDistance);

  } else {
  }
}

void button() {

  if (digitalRead(buttonpin) == LOW) {
    // Serial.println("Blind Stick Message");
    SendMessage();
  }
}

void rain() {

  int rain_state = digitalRead(DO_PIN);

  if (rain_state == HIGH) {
    // Serial.println("The rain is NOT detected");
    digitalWrite(13, LOW);
  } else {
    // Serial.println("The rain is detected");
    digitalWrite(13, HIGH);
  }
}

void SendMessage() {
  Serial.println("Blind Stick Message");
  mySerial.println("AT+CMGF=1");
  delay(1000);
  mySerial.println("AT+CMGS=\"+9188305848xx\"\r");
  //+918590668878
  delay(1000);
  mySerial.println("I Am In Problem Plz Help Me  https://www.google.com/maps?q=00.000000,00.00000000 ");
  delay(100);
  mySerial.println((char)26);
  delay(1000);
}