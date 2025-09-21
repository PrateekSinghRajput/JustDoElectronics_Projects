//Prateek
//https://justdoelectronics.com
//https://www.youtube.com/@JustDoElectronics/videos

#include <Arduino.h>
#include <TM1637Display.h>
#define key1 6
#define key2 7
#define key3 9
#define key4 8

#define CLK 4
#define DIO 5

int Buzzer = 12;
int ledPin = 11;

#define TEST_DELAY   2000
TM1637Display display(CLK, DIO);

void setup() {
  Serial.begin(9600);
  pinMode(key1, INPUT_PULLUP);
  pinMode(key2, INPUT_PULLUP);
  pinMode(key3, INPUT_PULLUP);
  pinMode(key4, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);
  pinMode(Buzzer, OUTPUT);

}

void loop() {
  int key1S = digitalRead(key1);
  int key2S = digitalRead(key2);
  int key3S = digitalRead(key3);
  int key4S = digitalRead(key4);

  if (!key1S) {
    digitalWrite(ledPin, LOW);
    digitalWrite(Buzzer, HIGH);
    Serial.println("key 1 is pressed");
    display.setBrightness(0x0f);
    uint8_t data[] = { 0x0, 0x0, 0x0, 0x0};
    display.setSegments(data);
    for (int i = 10; i >= 0; i--)
    {
      display.showNumberDec(i);
      delay(1000);

    }
    digitalWrite(ledPin, HIGH);
    delay(3000);
    digitalWrite(ledPin, LOW);
    digitalWrite(Buzzer, LOW);
    display.clear();

  }
  if (!key2S) {
    digitalWrite(ledPin, LOW);
    digitalWrite(Buzzer, HIGH);
    Serial.println("key 2 is pressed");
    display.setBrightness(0x0f);
    uint8_t data[] = { 0x0, 0x0, 0x0, 0x0};
    display.setSegments(data);
    for (int i = 20; i >= 0; i--)
    {
      display.showNumberDec(i);
      delay(1000);

    }
    digitalWrite(ledPin, HIGH);
    delay(3000);
    digitalWrite(ledPin, LOW);
    digitalWrite(Buzzer, LOW);
    display.clear();

  }

  if (!key3S) {
    digitalWrite(ledPin, LOW);
    digitalWrite(Buzzer, HIGH);
    Serial.println("key 3 is pressed");
    display.setBrightness(0x0f);
    uint8_t data[] = { 0x0, 0x0, 0x0, 0x0};
    display.setSegments(data);
    for (int i = 30; i >= 0; i--)

    {
      display.showNumberDec(i);
      delay(1000);

    }
    digitalWrite(ledPin, HIGH);
    delay(3000);
    digitalWrite(ledPin, LOW);
    digitalWrite(Buzzer, LOW);
    display.clear();

  }

  if (!key4S) {
    digitalWrite(ledPin, LOW);
    digitalWrite(Buzzer, HIGH);
    Serial.println("key 4 is pressed");
    display.setBrightness(0x0f);
    uint8_t data[] = { 0x0, 0x0, 0x0, 0x0};
    display.setSegments(data);
    for (int i = 40; i >= 0; i--)

    {
      display.showNumberDec(i);
      delay(1000);

    }
    digitalWrite(ledPin, HIGH);
    delay(3000);
    digitalWrite(ledPin, LOW);
    digitalWrite(Buzzer, LOW);
    display.clear();

  }

}



