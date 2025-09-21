#include "RCSwitch.h"
#include <EEPROM.h>
#include <TM1637Display.h>
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"

SoftwareSerial mySoftwareSerial(11, 12); // RX, TX
DFRobotDFPlayerMini myDFPlayer;
void printDetail(uint8_t type, int value);

#include <Wire.h>
#define CLK 10
#define DIO 9
uint8_t data[] = {0xf, 0xf, 0xf, 0xf};


// The amount of time (in milliseconds) between tests
#define TEST_DELAY   1000
TM1637Display display(CLK, DIO);
int Minutes = 2;
int Seconds = 0;
int Minutess = 6;
int Secondss = 0;
int Minutesss = 12;
int Secondsss = 0;
int Minutessss = 30;
int Secondssss = 0;


RCSwitch mySwitch = RCSwitch();

#define Relay1 8 // Load1 Pin Out

int Buzzer = 3;

#define key1 6 //connect wire 1 to pin 2
#define key2 7 //connect wire 2 to pin 3
#define key3 4 //connect wire 3 to pin 4
#define key4 5 //connect wire 4 to pin 5

int load1;

void setup() {

  Serial.begin(9600);
  mySoftwareSerial.begin(9600);

  Serial.println();
  Serial.println(F("Initializing DFPlayer..."));

  //Use softwareSerial to communicate with MP3
  if (!myDFPlayer.begin(mySoftwareSerial)) {
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    while (true);
  }
  Serial.println(F("DFPlayer Mini online."));

  //Set volume value (From 0 to 30)
  myDFPlayer.volume(30);



  mySwitch.enableReceive(0);  // Receiver on interrupt 0 => that is pin #2

  load1 = EEPROM.read(1);

  pinMode(key1, INPUT_PULLUP);// set pin as input
  pinMode(key2, INPUT_PULLUP);// set pin as input
  pinMode(key3, INPUT_PULLUP);// set pin as input
  pinMode(key4, INPUT_PULLUP);// set pin as input

  display.setBrightness(0x0f);
  uint8_t data[] = { 0x0, 0x0, 0x0, 0x0};
  display.setSegments(data);
  pinMode(Buzzer, OUTPUT);

  pinMode(Relay1, OUTPUT); digitalWrite(Relay1, load1);
  delay(100);
  display.setBrightness(0x0f);
  data[3] = display.encodeDigit(Seconds % 10);
  data[2] = display.encodeDigit(Seconds / 10);
  data[1] = display.encodeDigit(Seconds  % 10);
  data[0] = display.encodeDigit(Seconds  / 10);
  display.setSegments(data);
 
   myDFPlayer.play(2);
}

void loop() {

  if (mySwitch.available() > 0) {
    unsigned long int RF_data =  mySwitch.getReceivedValue() ;
    Serial.println(RF_data);

    switch (RF_data) {
      case 4456896: mySwitch.disableReceive();
        digitalWrite(Relay1, HIGH);
        Serial.println("key 1 is pressed");
        for (int i = 120; i > 0; i--) {
          display.setBrightness(0x0f);
          data[3] = display.encodeDigit(Seconds % 10);
          data[2] = display.encodeDigit(Seconds / 10);
          data[1] = display.encodeDigit(Minutes % 10);
          data[0] = display.encodeDigit(Minutes / 10);
          display.setSegments(data);
          uint8_t segto;
          segto = 0x80 | display.encodeDigit(Minutes % 10);
          display.setSegments(&segto, 1, 1);
          stepDown();
          delay(TEST_DELAY);
        }
        Minutes = 2;
        Seconds = 0;
        digitalWrite(Relay1, HIGH);
        digitalWrite(Buzzer, HIGH);
        delay(3000);
        digitalWrite(Relay1, LOW);
        digitalWrite(Buzzer, LOW);
        display.clear();
        myDFPlayer.play(1);
        display.setBrightness(0x0f);
        data[3] = display.encodeDigit(Seconds % 10);
        data[2] = display.encodeDigit(Seconds / 10);
        data[1] = display.encodeDigit(Seconds  % 10);
        data[0] = display.encodeDigit(Seconds  / 10);
        display.setSegments(data);
      

        break;


      case 4456752: mySwitch.disableReceive();
        digitalWrite(Relay1, HIGH);
        Serial.println("key 2 is pressed");

        for (int i = 360; i > 0; i--) {
          display.setBrightness(0x0f);
          data[3] = display.encodeDigit(Secondss % 10);
          data[2] = display.encodeDigit(Secondss / 10);
          data[1] = display.encodeDigit(Minutess % 10);
          data[0] = display.encodeDigit(Minutess / 10);
          display.setSegments(data);
          uint8_t segto;
          segto = 0x80 | display.encodeDigit(Minutess % 10);
          display.setSegments(&segto, 1, 1);
          stepDownn();
          delay(TEST_DELAY);
        }
        Minutess = 6;
        Secondss = 0;
        digitalWrite(Relay1, HIGH);
        digitalWrite(Buzzer, HIGH);
        delay(3000);
        digitalWrite(Relay1, LOW);
        digitalWrite(Buzzer, LOW);

        display.clear();
        myDFPlayer.play(1);
        display.setBrightness(0x0f);
        data[3] = display.encodeDigit(Seconds % 10);
        data[2] = display.encodeDigit(Seconds / 10);
        data[1] = display.encodeDigit(Seconds  % 10);
        data[0] = display.encodeDigit(Seconds  / 10);
        display.setSegments(data);
       
        break;

      case 4456716: mySwitch.disableReceive();
        digitalWrite(Relay1, HIGH);
        Serial.println("key 3 is pressed");
        for (int i = 720; i > 0; i--) {
          display.setBrightness(0x0f);
          data[3] = display.encodeDigit(Secondsss % 10);
          data[2] = display.encodeDigit(Secondsss / 10);
          data[1] = display.encodeDigit(Minutesss % 10);
          data[0] = display.encodeDigit(Minutesss / 10);
          display.setSegments(data);
          uint8_t segto;
          segto = 0x80 | display.encodeDigit(Minutesss % 10);
          display.setSegments(&segto, 1, 1);
          stepDownnn();
          delay(TEST_DELAY);
        }
        Minutesss = 12;
        Secondsss = 0;
        digitalWrite(Relay1, HIGH);
        digitalWrite(Buzzer, HIGH);
        delay(3000);
        digitalWrite(Relay1, LOW);
        digitalWrite(Buzzer, LOW);

        display.clear();
        myDFPlayer.play(1);
        display.setBrightness(0x0f);
        data[3] = display.encodeDigit(Seconds % 10);
        data[2] = display.encodeDigit(Seconds / 10);
        data[1] = display.encodeDigit(Seconds  % 10);
        data[0] = display.encodeDigit(Seconds  / 10);
        display.setSegments(data);
       
        break;

      case 4456707: mySwitch.disableReceive();
        digitalWrite(Relay1, HIGH);
        Serial.println("key 4 is pressed");
        for (int i = 1800; i > 0; i--) {
          display.setBrightness(0x0f);
          data[3] = display.encodeDigit(Secondssss % 10);
          data[2] = display.encodeDigit(Secondssss / 10);
          data[1] = display.encodeDigit(Minutessss % 10);
          data[0] = display.encodeDigit(Minutessss / 10);
          display.setSegments(data);
          uint8_t segto;
          segto = 0x80 | display.encodeDigit(Minutessss % 10);
          display.setSegments(&segto, 1, 1);
          stepDownnnn();
          delay(TEST_DELAY);
        }
        Minutessss = 30;
        Secondssss = 0;
        digitalWrite(Relay1, HIGH);
        digitalWrite(Buzzer, HIGH);
        delay(3000);
        digitalWrite(Relay1, LOW);
        digitalWrite(Buzzer, LOW);

        display.clear();
        myDFPlayer.play(1);
        display.setBrightness(0x0f);
        data[3] = display.encodeDigit(Seconds % 10);
        data[2] = display.encodeDigit(Seconds / 10);
        data[1] = display.encodeDigit(Seconds  % 10);
        data[0] = display.encodeDigit(Seconds  / 10);
        display.setSegments(data);
      
        break;

    }

    digitalWrite(Relay1, load1);

    unsigned long time_now = millis();
    int ck = 500;
    while (millis() < time_now + ck) {
      ;
    }
    mySwitch.resetAvailable();
    mySwitch.enableReceive(0);
  }
  int key1S = digitalRead(key1);// read if key1 is pressed
  int key2S = digitalRead(key2);// read if key1 is pressed
  int key3S = digitalRead(key3);// read if key1 is pressed
  int key4S = digitalRead(key4);// read if key1 is pressed

  if (!key1S) {
    digitalWrite(Relay1, HIGH);
    Serial.println("key 1 is pressed");
    for (int i = 120; i > 0; i--) {
      display.setBrightness(0x0f);
      data[3] = display.encodeDigit(Seconds % 10);
      data[2] = display.encodeDigit(Seconds / 10);
      data[1] = display.encodeDigit(Minutes % 10);
      data[0] = display.encodeDigit(Minutes / 10);
      display.setSegments(data);
      uint8_t segto;
      segto = 0x80 | display.encodeDigit(Minutes % 10);
      display.setSegments(&segto, 1, 1);
      stepDown();
      delay(TEST_DELAY);
    }
    Minutes = 2;
    Seconds = 0;
    digitalWrite(Relay1, HIGH);
    digitalWrite(Buzzer, HIGH);
    delay(3000);
    digitalWrite(Relay1, LOW);
    digitalWrite(Buzzer, LOW);

    display.clear();
    myDFPlayer.play(1);
    display.setBrightness(0x0f);
    data[3] = display.encodeDigit(Seconds % 10);
    data[2] = display.encodeDigit(Seconds / 10);
    data[1] = display.encodeDigit(Seconds  % 10);
    data[0] = display.encodeDigit(Seconds  / 10);
    display.setSegments(data);
    

  }
  if (!key2S) {

    digitalWrite(Relay1, HIGH);
    Serial.println("key 2 is pressed");
    for (int i = 360; i > 0; i--) {
      display.setBrightness(0x0f);
      data[3] = display.encodeDigit(Secondss % 10);
      data[2] = display.encodeDigit(Secondss / 10);
      data[1] = display.encodeDigit(Minutess % 10);
      data[0] = display.encodeDigit(Minutess / 10);
      display.setSegments(data);
      uint8_t segto;
      segto = 0x80 | display.encodeDigit(Minutess % 10);
      display.setSegments(&segto, 1, 1);
      stepDownn();
      delay(TEST_DELAY);
    }
    Minutess = 6;
    Secondss = 0;
    digitalWrite(Relay1, HIGH);
    digitalWrite(Buzzer, HIGH);
    delay(3000);
    digitalWrite(Relay1, LOW);
    digitalWrite(Buzzer, LOW);

    display.clear();
    myDFPlayer.play(1);
    display.setBrightness(0x0f);
    data[3] = display.encodeDigit(Seconds % 10);
    data[2] = display.encodeDigit(Seconds / 10);
    data[1] = display.encodeDigit(Seconds  % 10);
    data[0] = display.encodeDigit(Seconds  / 10);
    display.setSegments(data);
    

  }

  if (!key3S) {

    digitalWrite(Relay1, HIGH);
    Serial.println("key 3 is pressed");
    for (int i = 720; i > 0; i--) {
      display.setBrightness(0x0f);
      data[3] = display.encodeDigit(Secondsss % 10);
      data[2] = display.encodeDigit(Secondsss / 10);
      data[1] = display.encodeDigit(Minutesss % 10);
      data[0] = display.encodeDigit(Minutesss / 10);
      display.setSegments(data);
      uint8_t segto;
      segto = 0x80 | display.encodeDigit(Minutesss % 10);
      display.setSegments(&segto, 1, 1);
      stepDownnn();
      delay(TEST_DELAY);
    }
    Minutesss = 12;
    Secondsss = 0;
    digitalWrite(Relay1, HIGH);
    digitalWrite(Buzzer, HIGH);
    delay(3000);
    digitalWrite(Relay1, LOW);
    digitalWrite(Buzzer, LOW);

    display.clear();
    myDFPlayer.play(1);
    display.setBrightness(0x0f);
    data[3] = display.encodeDigit(Seconds % 10);
    data[2] = display.encodeDigit(Seconds / 10);
    data[1] = display.encodeDigit(Seconds  % 10);
    data[0] = display.encodeDigit(Seconds  / 10);
    display.setSegments(data);

  }

  if (!key4S) {
    digitalWrite(Relay1, HIGH);
    Serial.println("key 4 is pressed");
    for (int i = 1800; i > 0; i--) {
      display.setBrightness(0x0f);
      data[3] = display.encodeDigit(Secondssss % 10);
      data[2] = display.encodeDigit(Secondssss / 10);
      data[1] = display.encodeDigit(Minutessss % 10);
      data[0] = display.encodeDigit(Minutessss / 10);
      display.setSegments(data);
      uint8_t segto;
      segto = 0x80 | display.encodeDigit(Minutessss % 10);
      display.setSegments(&segto, 1, 1);
      stepDownnnn();
      delay(TEST_DELAY);
    }
    Minutessss = 30;
    Secondssss = 0;
    digitalWrite(Relay1, HIGH);
    digitalWrite(Buzzer, HIGH);
    delay(3000);
    digitalWrite(Relay1, LOW);
    digitalWrite(Buzzer, LOW);

    display.clear();
    myDFPlayer.play(1);
    display.setBrightness(0x0f);
    data[3] = display.encodeDigit(Seconds % 10);
    data[2] = display.encodeDigit(Seconds / 10);
    data[1] = display.encodeDigit(Seconds  % 10);
    data[0] = display.encodeDigit(Seconds  / 10);
    display.setSegments(data);
 

  }

}
void stepDown() {
  if (Seconds > 0) {
    Seconds -= 1;
  } else {
    if (Minutes > 0) {
      Seconds = 59;
      Minutes -= 1;
    } else {

    }
  }
}
void stepDownn() {
  if (Secondss > 0) {
    Secondss -= 1;
  } else {
    if (Minutess > 0) {
      Secondss = 59;
      Minutess -= 1;
    } else {

    }
  }
}
void stepDownnn() {
  if (Secondsss > 0) {
    Secondsss -= 1;
  } else {
    if (Minutesss > 0) {
      Secondsss = 59;
      Minutesss -= 1;
    } else {

    }
  }
}
void stepDownnnn() {
  if (Secondssss > 0) {
    Secondssss -= 1;
  } else {
    if (Minutessss > 0) {
      Secondssss = 59;
      Minutessss -= 1;
    } else {

    }
  }
}
