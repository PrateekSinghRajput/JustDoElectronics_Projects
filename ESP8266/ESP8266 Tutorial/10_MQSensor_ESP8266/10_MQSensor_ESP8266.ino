

void setup() {
  Serial.begin(115200);
  pinMode(D5, OUTPUT);
}
void loop() {
  int analog = analogRead(A0);
    Serial.println(analog);

  if (analog < 300) {
    digitalWrite(D5, HIGH);
    Serial.println("Normal");
  }
  else {
    digitalWrite(D5, LOW);
    Serial.println("GAS Detected");
  }
  delay(100);
}
