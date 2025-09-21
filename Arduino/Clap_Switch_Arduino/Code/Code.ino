#define relay 3
const int microphonePin = A0;
boolean LEDStatus = false;

void setup() {
  Serial.begin(9600);
  pinMode(relay, OUTPUT);
}

void loop() {
  int mn = 1024;
  int mx = 0;

  for (int i = 0; i < 10000; ++i) {

    int val = analogRead(microphonePin);

    mn = min(mn, val);
    mx = max(mx, val);
  }

  int delta = mx - mn;

  Serial.print("Min=");
  Serial.print(mn);
  Serial.print(" Max=");
  Serial.print(mx);
  Serial.print(" Delta=");
  Serial.println(delta);

  if (mx > 70) {

    if (LEDStatus == false) {
      LEDStatus = true;
      digitalWrite(relay, HIGH);
    } else if (LEDStatus == true) {
      LEDStatus = false;
      digitalWrite(relay, LOW);
    }
  }
  delay(100);
}