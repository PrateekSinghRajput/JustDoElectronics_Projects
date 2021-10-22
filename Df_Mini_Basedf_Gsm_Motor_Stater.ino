//Pratek
//www.prateeks.in

#include <EEPROM.h>
#include <SoftwareSerial.h> 
SoftwareSerial GSM(8, 9);  

#include <DFPlayer_Mini_Mp3.h>

char *phone_no = "9975617490"; 

unsigned long currentTime;
unsigned long loopTime1;
unsigned long loopTime2;

#define Relay1 2 
#define Relay2 3 


int load1, load2;

void setup() {

pinMode(Relay1, OUTPUT); 
digitalWrite(Relay1, 1); 
pinMode(Relay2, OUTPUT);
digitalWrite(Relay2, 1); 


Serial.begin(9600);
GSM.begin(9600); 

mp3_set_serial (Serial);
mp3_set_volume (29);

//Serial.println("Initializing....");
initModule("AT","OK",300);        
initModule("ATE0","OK",300);
initModule("AT+CPIN?","READY",300);
initModule("AT+CLIP=1","OK",300);  
initModule("AT+DDET=1","OK",300);
//Serial.println("Initialized Successfully");  

load1 = EEPROM.read(1);
load2 = EEPROM.read(2);


relays();
}

void loop(){
currentTime = millis(); 
if(currentTime >= (loopTime1 + 2000)){
initModule("AT","OK",300);   
loopTime1 = currentTime;
}
    
if(GSM.find("CLIP:")) {
 //Serial.println("RING!");
 delay(200);

while(1){     
if(GSM.find(phone_no)){ 
initModule("ATA","OK",300);      
delay(1500); 
mp3_play (28);
delay(4000); 
mp3_play (0);
delay(3000);
loopTime2 = currentTime;
while (1){   
   currentTime = millis();  
if(currentTime >= (loopTime2 + 30000)){
  //Serial.println("Call End");
  loopTime2 = currentTime; 
  break;
}       

if(GSM.find("+DTMF:")){
  int Data = GSM.parseInt();
  switch(Data){                                

case 1: {
   initModule("AT+DDET=0","OK",300);  
   load1=!load1;
   eeprom_write();
   relays();
    if(load1==0){mp3_play (1);}
            else{mp3_play (2);}       
   initModule("AT+DDET=1","OK",300);
loopTime2 = currentTime;    
   break;}

case 2: {
   initModule("AT+DDET=0","OK",300);  
   load2=!load2;
   eeprom_write();
   relays();
    if(load2==0){mp3_play (3);}
            else{mp3_play (4);}       
   initModule("AT+DDET=1","OK",300);
loopTime2 = currentTime;   
   break;}



case 8: {
   initModule("AT+DDET=0","OK",300);  
   load1=0,load2=0;
   eeprom_write();
   relays();
   mp3_play (9);   
   initModule("AT+DDET=1","OK",300); 
loopTime2 = currentTime;   
   break;}

case 0: {
   initModule("AT+DDET=0","OK",300);  
   load1=1,load2=1;
   eeprom_write();
   relays();
   mp3_play (10);    
   initModule("AT+DDET=1","OK",300);   
loopTime2 = currentTime;   
   break;}

case 9: {
   initModule("AT+DDET=0","OK",300);  
   if(load1==0 && load2==0){ 
      mp3_play (19); 
   }else{   
    if(load1==1 && load2==1){
      mp3_play (20);
    }else{
     if(load1==0){mp3_play (11);}
             else{mp3_play (12);}
     delay(2000);
     if(load2==0){mp3_play (13);}
             else{mp3_play (14);}
        
    }
   } 
   initModule("AT+DDET=1","OK",300);   
loopTime2 = currentTime;   
   break;}

case 7: {
initModule("AT+DDET=0","OK",300);     
mp3_play (23);delay(2000);    
mp3_play (27);
initModule("AT+DDET=1","OK",300); 
loopTime2 = currentTime;    
   break;}
    } 
   }
  }   
 } 
else {
initModule("AT","OK",300); 
break;            
 } 
}
 
initModule("AT+CHUP;","OK",300); 
 }
}

void eeprom_write(){
EEPROM.write(1,load1);
EEPROM.write(2,load2);
  
}

void relays(){  
digitalWrite(Relay1, load1); 
digitalWrite(Relay2, load2); 

}

 // Init GSM Module 
void initModule(String cmd, char *res, int t){
while(1){
    //Serial.println(cmd);
    GSM.println(cmd);
    delay(100);
    while(GSM.available()>0){
       if(GSM.find(res)){
        //Serial.println(res);
        delay(t);
        return;
       }else{
       }}
    delay(t);
  }
}   
