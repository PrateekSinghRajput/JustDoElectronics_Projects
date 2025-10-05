#include <SoftwareSerial.h>

SoftwareSerial gsm(2, 3);  // RX = 2, TX = 3

int LED = 8;
int motor = 9;

int temp = 0;
int i = 0;
char str[15];

void setup() {
  Serial.begin(9600);
  gsm.begin(9600);

  pinMode(motor, OUTPUT);
  pinMode(LED, OUTPUT);

  digitalWrite(motor, LOW);
  digitalWrite(LED, LOW);

  delay(60000);  // Wait for GSM module to initialize

  gsm.println("AT+CNMI=2,2,0,0,0");
  delay(1000);
  gsm.println("AT+CMGF=1");
  delay(500);

  sendSMS("System is ready to receive commands.");
}

void loop() {
  if (temp == 1) {
    checkCommand();
    temp = 0;
    i = 0;
    delay(1000);
  }
}

void serialEvent() {
  while (gsm.available()) {
    if (gsm.find("/")) {
      delay(1000);
      while (gsm.available()) {
        char inChar = gsm.read();
        if (i < sizeof(str) - 1) {
          str[i++] = inChar;
          if (inChar == '/') {
            str[i] = '\0';  // Null-terminate string
            temp = 1;
            return;
          }
        }
      }
    }
  }
}

void sendSMS(const char *message) {
  gsm.println("AT+CMGS=\"+91xxxxxxxxxx\"");
  delay(1000);
  gsm.println(message);
  delay(100);
  gsm.write(26);  // Ctrl+Z to send SMS
  delay(1000);
}

void checkCommand() {
  if (strncmp(str, "motor on", 8) == 0) {
    digitalWrite(motor, HIGH);
    digitalWrite(LED, HIGH);
    sendSMS("Motor Activated");
  } else if (strncmp(str, "motor off", 9) == 0) {
    digitalWrite(motor, LOW);
    digitalWrite(LED, LOW);
    sendSMS("Motor Deactivated");
  } else if (strncmp(str, "test", 4) == 0) {
    sendSMS("The System is Working Fine.");
  }
}
