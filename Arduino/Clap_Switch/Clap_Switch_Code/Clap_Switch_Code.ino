int SoundSensor = A0;
int LED = 13;
int RELAY = 3;
boolean LEDStatus = false;

void setup() {
  pinMode(SoundSensor, INPUT);
  pinMode(LED, OUTPUT);
  pinMode(RELAY, OUTPUT);
  Serial.begin(9600);
  digitalWrite(RELAY, HIGH);
}

void loop() {
  int SensorData = digitalRead(SoundSensor);
  Serial.println(SensorData);
  if (SensorData == 1) {
    if (LEDStatus == false) {
      LEDStatus = true;
      digitalWrite(LED, HIGH);
      digitalWrite(RELAY, LOW);
    } else if (LEDStatus == true) {
      LEDStatus = false;
      digitalWrite(LED, LOW);
      digitalWrite(RELAY, HIGH);
    }
  }
}