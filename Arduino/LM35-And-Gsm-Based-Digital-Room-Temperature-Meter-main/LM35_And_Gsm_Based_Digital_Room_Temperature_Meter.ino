 
//PRATEEK
//www.prateeks.in

#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 20, 4);
#include <SoftwareSerial.h>
 
SoftwareSerial mySerial(9,8);

const int Buzzer=5; 
const int LED_GREEN=3; 
const int RELAY=6; 

const int up_key=4;
const int down_key=2;

int SetPoint=35;

void setup(){

  mySerial.begin(9600); 
  Serial.begin(9600);
  pinMode(Buzzer,OUTPUT);
  pinMode(LED_GREEN,OUTPUT);  
  pinMode(RELAY,OUTPUT);  
  pinMode(up_key,INPUT);
  pinMode(down_key,INPUT);
  
  //Pull up for setpoint keys
  digitalWrite(up_key,HIGH);
  digitalWrite(down_key,HIGH);
   
lcd.init();
lcd.backlight();
lcd.setCursor(2,0);
lcd.print ("WELCOME TO  ");
lcd.setCursor(2,1);
lcd.print ("JUSTDOELECTRONICS");
lcd.setCursor(6,2);
lcd.print ("DESIGN By :-");
lcd.setCursor(6,3);
lcd.print ("PRATEEK");
delay(3000);
lcd.clear(); 
  
  
  digitalWrite(LED_GREEN,HIGH);  
  digitalWrite(Buzzer,LOW);     
  digitalWrite(RELAY,LOW);       
  delay(2000);
}
 
void loop(){
  double Temperature = ((5.0/1024.0) * analogRead(A0)) * 100;  
 
  lcd.setCursor(1,0);
  lcd.print("Temperature:");    
  lcd.print(Temperature);
  
//Get user input for setpoints  
  if(digitalRead(down_key)==LOW)
  {
    if(SetPoint>0)
    {
      SetPoint--;    
    }
  }
  if(digitalRead(up_key)==LOW)
  {
    if(SetPoint<150)
    {
      SetPoint++;
    }
  }

  lcd.setCursor(2,1);
  lcd.print("Set Point:");
  lcd.print(SetPoint);
  lcd.print("C   ");

if(Temperature < SetPoint)
{
   digitalWrite(RELAY,LOW);    
   digitalWrite(Buzzer,LOW);
   digitalWrite(LED_GREEN,HIGH);  

lcd.setCursor(1,2);
lcd.print ("Exhaust fan :-OFF");


}
else
{
  digitalWrite(RELAY,HIGH);   
  digitalWrite(LED_GREEN,LOW);
  digitalWrite(Buzzer,HIGH); 


lcd.setCursor(1,2);
lcd.print ("Exhaust fan :- ON");

Serial.println("I am in send");
mySerial.println("AT+CMGF=1"); 
delay(1000); 
mySerial.println("AT+CMGS=\"+918830584864\"\r"); 
delay(1000);
Temperature/mySerial.println("Temperature High Plz Check");
delay(100);
mySerial.println((char)26);
delay(1000);
  
}

  delay(100); 
}
