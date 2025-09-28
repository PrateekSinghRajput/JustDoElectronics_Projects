int count = 0;
char card_no[12];

void setup() {
  Serial.begin(9600);
}

void loop() {
  if (Serial.available()) {
    count = 0;
    while (Serial.available() && count < 12) {
      card_no[count] = Serial.read();
      count++;
      delay(5);
    }
    Serial.print(card_no);
  }
}