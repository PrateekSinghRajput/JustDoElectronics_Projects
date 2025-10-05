//Prateek
//https://justdoelectronics.com
//https://www.youtube.com/@JustDoElectronics/videos

#define Start 8
#define Stop 7
#define Run 6

#define Red 2
#define Yellow 3
#define Green 4

int temp = 0, i = 0;
int led = 13;
char str[15];
void setup() {
  Serial.begin(9600);
  pinMode(led, OUTPUT);
  pinMode(Start, OUTPUT);
  pinMode(Stop, OUTPUT);
  pinMode(Run, OUTPUT);

  pinMode(Red, OUTPUT);
  pinMode(Yellow, OUTPUT);
  pinMode(Green, OUTPUT);

  digitalWrite(Start, HIGH);
  digitalWrite(Stop, HIGH);
  digitalWrite(Run, HIGH);
  Serial.println("AT+CNMI=2,2,0,0,0");
  delay(400);
  Serial.println("AT+CMGF=1");
  delay(400);
}
void loop() {
  if (temp == 1) {
    check();
    temp = 0;
    i = 0;
    delay(1000);
  }
}
void serialEvent() {
  while (Serial.available()) {
    if (Serial.find("#A.")) {
      digitalWrite(led, HIGH);
      delay(1000);
      digitalWrite(led, LOW);
      while (Serial.available()) {
        char inChar = Serial.read();
        str[i++] = inChar;
        if (inChar == '*') {
          temp = 1;
          return;
        }
      }
    }
  }
}
void check() {
  if (!(strncmp(str, "start", 5))) {
    digitalWrite(Start, LOW);
    digitalWrite(Green, HIGH);
    delay(3000);
    digitalWrite(Green, LOW);
    digitalWrite(Start, HIGH);
  }

  else if (!(strncmp(str, "run", 3))) {
    digitalWrite(Run, LOW);
    digitalWrite(Red, HIGH);
    delay(3000);
    digitalWrite(Red, LOW);
    digitalWrite(Run, HIGH);
  }

  else if (!(strncmp(str, "stop", 4))) {
    digitalWrite(Stop, LOW);
    digitalWrite(Yellow, HIGH);
    delay(3000);
    digitalWrite(Yellow, LOW);
    digitalWrite(Stop, HIGH);
  }
}
