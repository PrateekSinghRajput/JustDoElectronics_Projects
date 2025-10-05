void setup()  
 {  
   pinMode(16,HIGH);   
   pinMode(5,HIGH);  
   pinMode(4,HIGH);    
 }  
 void loop()   
 {  
    // BLUE LED ON  
    digitalWrite(16,LOW);  
    digitalWrite(5,HIGH);  
    digitalWrite(4,HIGH);  
    delay(1000);  
    // GREEN LED ON  
    digitalWrite(16,HIGH);  
    digitalWrite(5,LOW);  
    digitalWrite(4,HIGH);  
    delay(1000);  
    // RED LED ON  
    digitalWrite(16,HIGH);  
    digitalWrite(5,HIGH);  
    digitalWrite(4,LOW);  
    delay(1000);  
 }  