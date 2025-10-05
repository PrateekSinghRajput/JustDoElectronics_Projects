//Code 1

void setup() {
  Serial.begin(115200);
}

void loop() {
  Serial.println(analogRead(A0));
  delay(100);
}


/*
Code 2

void setup(){
  pinMode(D0, OUTPUT);
}

void loop(){
  analogWrite(D0, analogRead(A0)/4);
}

*/