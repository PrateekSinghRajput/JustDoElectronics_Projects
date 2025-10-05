//Blinking an LED 

void setup(){
  pinMode(D0, OUTPUT);
}

void loop(){
  digitalWrite(D0, HIGH);
  delay(1000);
  digitalWrite(D0, LOW);
  delay(1000);  
}



/*
void setup(){
  Serial.begin(115200);
  pinMode(D0, OUTPUT);
}

void loop(){
  Serial.println("");
  for(int i=0; i<256; i++){
    analogWrite(D0, i);
    Serial.print("####");
    delay(10);
  }
}
/*