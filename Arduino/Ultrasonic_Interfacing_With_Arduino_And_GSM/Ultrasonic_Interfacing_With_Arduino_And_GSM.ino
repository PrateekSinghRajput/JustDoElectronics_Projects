int const trigPin = 10;
int const echoPin = 9;
int const buzzPin = 13;

void setup() {
  Serial.begin(9600);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(buzzPin, OUTPUT);
}

void loop() {
  long duration;
  int distance;

  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);

  distance = duration * 0.034 / 2;

  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  if (distance > 0 && distance <= 20) {
    digitalWrite(buzzPin, HIGH);
    delay(1000);
    SendSMS();
  } else {
    digitalWrite(buzzPin, LOW);
  }

  delay(200);
}

void SendSMS() {
  Serial.println("AT+CMGF=1");
  delay(1000);
  Serial.println("AT+CMGS=\"+91xxxxxxxxxx\"");
  delay(1000);
  Serial.println("Sensor Detect Movement Plz Check");
  delay(500);
  Serial.write(26);
  delay(3000);
}
