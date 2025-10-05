#include <EEPROM.h>
#include <SoftwareSerial.h>
#include <DFPlayer_Mini_Mp3.h>

// GSM serial pins
SoftwareSerial GSM(2, 3);  // RX, TX

// DFPlayer Mini MP3 control pins (uses Serial)
#define MP3_RX 10  // Arduino RX to DFPlayer TX (example)
#define MP3_TX 11  // Arduino TX to DFPlayer RX (example)
SoftwareSerial mp3Serial(MP3_RX, MP3_TX);

char phone_no[] = "9975617490";

unsigned long currentTime;
unsigned long loopTime1;
unsigned long loopTime2;

#define Relay1 5
#define Relay2 6

int load1, load2;

void setup() {
  pinMode(Relay1, OUTPUT);
  digitalWrite(Relay1, HIGH);  // Relay off assumed active low
  pinMode(Relay2, OUTPUT);
  digitalWrite(Relay2, HIGH);

  Serial.begin(9600);
  GSM.begin(9600);

  mp3Serial.begin(9600);
  mp3_set_serial(mp3Serial);
  mp3_set_volume(29);

  EEPROM.begin(512);  // Adjust EEPROM size based on platform

  Serial.println("Initializing....");
  initModule("AT", "OK", 300);
  initModule("ATE0", "OK", 300);
  initModule("AT+CPIN?", "READY", 300);
  initModule("AT+CLIP=1", "OK", 300);
  initModule("AT+DDET=1", "OK", 300);
  Serial.println("Initialized Successfully");

  load1 = EEPROM.read(1);
  load2 = EEPROM.read(2);

  relays();
}

void loop() {
  currentTime = millis();

  if (currentTime >= (loopTime1 + 2000)) {
    initModule("AT", "OK", 300);
    loopTime1 = currentTime;
  }

  if (GSM.find("CLIP:")) {
    Serial.println("RING!");
    delay(200);

    while (true) {
      if (GSM.find(phone_no)) {
        initModule("ATA", "OK", 300);
        delay(1500);
        mp3_play(28);
        delay(4000);
        mp3_play(0);
        delay(3000);
        loopTime2 = millis();

        while (true) {
          currentTime = millis();
          if (currentTime >= (loopTime2 + 30000)) {
            Serial.println("Call End");
            loopTime2 = currentTime;
            break;
          }

          if (GSM.find("+DTMF:")) {
            int Data = GSM.parseInt();
            switch (Data) {
              case 1:
                initModule("AT+DDET=0", "OK", 300);
                load1 = !load1;
                eeprom_write();
                relays();
                if (load1 == 0) {
                  mp3_play(1);
                } else {
                  mp3_play(2);
                }
                initModule("AT+DDET=1", "OK", 300);
                loopTime2 = currentTime;
                break;

              case 2:
                initModule("AT+DDET=0", "OK", 300);
                load2 = !load2;
                eeprom_write();
                relays();
                if (load2 == 0) {
                  mp3_play(3);
                } else {
                  mp3_play(4);
                }
                initModule("AT+DDET=1", "OK", 300);
                loopTime2 = currentTime;
                break;

              case 3:
                initModule("AT+DDET=0", "OK", 300);
                load1 = 0;
                load2 = 0;
                eeprom_write();
                relays();
                mp3_play(9);
                initModule("AT+DDET=1", "OK", 300);
                loopTime2 = currentTime;
                break;

              case 0:
                initModule("AT+DDET=0", "OK", 300);
                load1 = 1;
                load2 = 1;
                eeprom_write();
                relays();
                mp3_play(10);
                initModule("AT+DDET=1", "OK", 300);
                loopTime2 = currentTime;
                break;

              case 4:
                initModule("AT+DDET=0", "OK", 300);
                if (load1 == 0 && load2 == 0) {
                  mp3_play(19);
                } else if (load1 == 1 && load2 == 1) {
                  mp3_play(20);
                } else {
                  if (load1 == 0) {
                    mp3_play(11);
                  } else {
                    mp3_play(12);
                  }
                  delay(2000);
                  if (load2 == 0) {
                    mp3_play(13);
                  } else {
                    mp3_play(14);
                  }
                }
                initModule("AT+DDET=1", "OK", 300);
                loopTime2 = currentTime;
                break;

              case 5:
                initModule("AT+DDET=0", "OK", 300);
                mp3_play(23);
                delay(2000);
                mp3_play(27);
                initModule("AT+DDET=1", "OK", 300);
                loopTime2 = currentTime;
                break;
            }
          }
        }
      } else {
        initModule("AT", "OK", 300);
        break;
      }
    }
    initModule("AT+CHUP;", "OK", 300);
  }
}

void eeprom_write() {
  EEPROM.write(1, load1);
  EEPROM.write(2, load2);
  EEPROM.commit();  // Commit changes to EEPROM
}

void relays() {
  digitalWrite(Relay1, load1);
  digitalWrite(Relay2, load2);
}

// Init GSM Module
void initModule(String cmd, const char *res, int t) {
  while (true) {
    GSM.println(cmd);
    delay(100);
    while (GSM.available() > 0) {
      if (GSM.find(res)) {
        delay(t);
        return;
      }
    }
    delay(t);
  }
}
