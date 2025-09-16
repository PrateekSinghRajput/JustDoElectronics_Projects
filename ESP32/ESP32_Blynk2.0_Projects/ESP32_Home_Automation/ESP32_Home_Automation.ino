
//Prateek
//www.justdoelectronics.com
//https://www.youtube.com/c/JustDoElectronics

#define BLYNK_TEMPLATE_ID "TMPLjOi0Rz-W"
#define BLYNK_DEVICE_NAME "esp32"
#define BLYNK_AUTH_TOKEN "UF3ok0_l0VkhTDS-WD1gWZ65DdhnHpwx"

// Comment this out to disable prints and save space
#define BLYNK_PRINT Serial


#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

char auth[] = BLYNK_AUTH_TOKEN;

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "prateeksingh";
char pass[] = "singh@@12345";

BlynkTimer timer;


#define button1_pin 5
#define button2_pin 10
#define button3_pin 19
#define button4_pin 21

#define led1_pin 13
#define led2_pin 14
#define led3_pin 26
#define led4_pin 33

int led1_state = 0;
int led2_state = 0;
int led3_state = 0;
int led4_state = 0;

#define button1_vpin    V1
#define button2_vpin    V2
#define button3_vpin    V3 
#define button4_vpin    V4

BLYNK_CONNECTED() {
  Blynk.syncVirtual(button1_vpin);
  Blynk.syncVirtual(button2_vpin);
  Blynk.syncVirtual(button3_vpin);
  Blynk.syncVirtual(button4_vpin);
}

BLYNK_WRITE(button1_vpin) {
  led1_state = param.asInt();
  digitalWrite(led1_pin, led1_state);
}

BLYNK_WRITE(button2_vpin) {
  led2_state = param.asInt();
  digitalWrite(led2_pin, led2_state);
}

BLYNK_WRITE(button3_vpin) {
  led3_state = param.asInt();
  digitalWrite(led3_pin, led3_state);
}

BLYNK_WRITE(button4_vpin) {
  led4_state = param.asInt();
  digitalWrite(led4_pin, led4_state);
}


void setup()
{
  // Debug console
  Serial.begin(115200);
  pinMode(button1_pin, INPUT_PULLUP);
  pinMode(button2_pin, INPUT_PULLUP);
  pinMode(button3_pin, INPUT_PULLUP);
  pinMode(button4_pin, INPUT_PULLUP);
  
  pinMode(led1_pin, OUTPUT);
  pinMode(led2_pin, OUTPUT);
  pinMode(led3_pin, OUTPUT);
  pinMode(led4_pin, OUTPUT);


  digitalWrite(led1_pin, HIGH);
  digitalWrite(led2_pin, HIGH);
  digitalWrite(led3_pin, HIGH);
  digitalWrite(led4_pin, HIGH);
  
  Blynk.begin(auth, ssid, pass);
 
}

void loop()
{
  Blynk.run();
  timer.run();
  
  listen_push_buttons();
}

void listen_push_buttons(){
    if(digitalRead(button1_pin) == LOW){
      delay(200);
      control_led(1);
      Blynk.virtualWrite(button1_vpin, led1_state); //update button state
    }
    else if (digitalRead(button2_pin) == LOW){
      delay(200);
      control_led(2);
      Blynk.virtualWrite(button2_vpin, led2_state); //update button state
    }
    else if (digitalRead(button3_pin) == LOW){
      delay(200);
      control_led(3);
      Blynk.virtualWrite(button3_vpin, led3_state); //update button state
    }
    else if (digitalRead(button4_pin) == LOW){
      delay(200);
      control_led(4);
      Blynk.virtualWrite(button4_vpin, led4_state); //update button state
    }
}

void control_led(int led){

  if(led == 1){
    led1_state = !led1_state;
    digitalWrite(led1_pin, led1_state);
    Serial.print("led1 State = ");
    Serial.println(led1_state);
    delay(50);
  }

  else if(led == 2){
    led2_state = !led2_state;
    digitalWrite(led2_pin, led2_state);
    delay(50);
  }
  
  else if(led == 3){
    led3_state = !led3_state;
    digitalWrite(led3_pin, led3_state);
    delay(50);
  }

  else if(led == 4){
    led4_state = !led4_state;
    digitalWrite(led4_pin, led4_state);
    delay(50);
  }
  
}