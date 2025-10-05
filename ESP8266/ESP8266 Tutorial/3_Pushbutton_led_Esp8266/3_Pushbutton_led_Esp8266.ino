int ledpin = 4; // D2
int button = 2; //D4
int buttonState=0;
void setup() {
 pinMode(ledpin, OUTPUT);
 pinMode(button, INPUT);
}
void loop() {
 buttonState=digitalRead(button); 
 if (buttonState == 1)
 {
 digitalWrite(ledpin, HIGH); 
 delay(200);
 }
 if (buttonState==0)
 {
 digitalWrite(ledpin, LOW); 
 delay(200);
 }
}
