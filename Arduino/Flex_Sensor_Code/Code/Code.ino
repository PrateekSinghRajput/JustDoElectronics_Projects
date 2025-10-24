
//Prateek
//https://www.youtube.com/c/JustDoElectronics/videos

#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);
SoftwareSerial mySoftwareSerial(2, 3);
DFRobotDFPlayerMini myDFPlayer;

void setup() {
  lcd.init();
  lcd.backlight();

  lcd.setCursor(0, 0);
  lcd.print("Sign Language");
  lcd.setCursor(0, 1);
  lcd.print("To Speech");
  delay(3000);
  lcd.clear();

  Serial.begin(9600);
  mySoftwareSerial.begin(9600);

  Serial.println("Initializing DFPlayer...");
  if (!myDFPlayer.begin(mySoftwareSerial)) {
    Serial.println("Unable to begin DFPlayer");
    while (true)
      ;
  }
  Serial.println("DFPlayer initialized");
  myDFPlayer.volume(30);

  // Set analog inputs
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
  pinMode(A3, INPUT);
}

void loop() {
  // Read sensor values
  int sensor1 = analogRead(A0);
  int sensor2 = analogRead(A1);
  int sensor3 = analogRead(A2);
  int sensor4 = analogRead(A3);

  Serial.print("Sensor1: ");
  Serial.print(sensor1);
  Serial.print("\tSensor2: ");
  Serial.print(sensor2);
  Serial.print("\tSensor3: ");
  Serial.println(sensor3);
  Serial.print("\tSensor4: ");
  Serial.println(sensor4);

  if (sensor1 >= 772) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("I Am Hungry");
    myDFPlayer.play(1);
    delay(3000);
  }

  if (sensor2 >= 730) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Help Me To Rich Home");
    myDFPlayer.play(3);
    delay(3000);
  }

  if (sensor3 >= 720) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Call The Police");
    myDFPlayer.play(5);
    delay(3000);
  }


  if (sensor4 >= 738) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("I Need Help");
    myDFPlayer.play(7);
    delay(3000);
  }

  else {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Nothing Detected");
    delay(1000);
  }

  delay(50);
}
