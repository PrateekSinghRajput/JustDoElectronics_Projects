#include <ESP32Servo.h>

#include <esp_now.h>
#include <WiFi.h>

#define in1 26
#define in2 27
#define in3 14
#define in4 12

#define spd 32
#define rely1 4

#define sev1 25
#define sev2 5

Servo s1;
Servo s2;

int sa1 = 90;
int sa2 = 90;

// Setting PWM properties
const int freq = 30000;
const int pwmChannel = 2;
const int resolution = 8;

typedef struct struct_message {
  int x;
  int y;
  int swi;
  int front;
  int back;
  int left;
  int right;
  int speed;
} struct_message;
struct_message data;

void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&data, incomingData, sizeof(data));

  ledcWrite(pwmChannel, data.speed);

  if(data.back == 0){
    digitalWrite(in1,HIGH);
    digitalWrite(in2,LOW);
    digitalWrite(in3,HIGH);
    digitalWrite(in4,LOW);
  }

  else if(data.front == 0){
    digitalWrite(in1,LOW);
    digitalWrite(in2,HIGH);
    digitalWrite(in3,LOW);
    digitalWrite(in4,HIGH);
  }

  else if(data.right == 0){
    digitalWrite(in1,LOW);
    digitalWrite(in2,HIGH);
    digitalWrite(in3,HIGH);
    digitalWrite(in4,LOW);
  }

  else if(data.left == 0){
    digitalWrite(in1,HIGH);
    digitalWrite(in2,LOW);
    digitalWrite(in3,LOW);
    digitalWrite(in4,HIGH);
  }
  else{
    digitalWrite(in1,LOW);
    digitalWrite(in2,LOW);
    digitalWrite(in3,LOW);
    digitalWrite(in4,LOW);
  }
  
  if (data.swi == 1){
    digitalWrite(rely1, LOW);
  }
  else{
    digitalWrite(rely1, HIGH);
  }

  if(data.x > 0 ){
    s1.write(180);
    s2.write(180);
  }
  else if(data.x < 4000){
    s1.write(90);
    s2.write(90);
  }



  Serial.print(data.x);
  Serial.print("\t");
  Serial.println(data.y);

}
 
void setup() {
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_register_recv_cb(OnDataRecv);

  pinMode(in1,OUTPUT);
  pinMode(in2,OUTPUT);
  pinMode(in3,OUTPUT);
  pinMode(in4,OUTPUT);
  pinMode(spd,OUTPUT);
  pinMode(rely1,OUTPUT);

  s1.attach(sev1);
  s2.attach(sev2);

  s1.write(sa1);
  s2.write(sa2);

  ledcSetup(pwmChannel, freq, resolution);
  ledcAttachPin(spd, pwmChannel);

}
 
void loop() {

}