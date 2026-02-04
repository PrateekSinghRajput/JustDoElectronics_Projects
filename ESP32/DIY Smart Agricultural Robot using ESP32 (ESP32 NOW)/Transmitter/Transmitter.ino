#include <esp_now.h>
#include <WiFi.h>

#define vrx 32
#define vry 33
#define sw 12
#define pot 35
#define fwd 4
#define rev 14
#define lef 26
#define rit 27

uint8_t broadcastAddress[] = {0x48, 0xE7, 0x29, 0xB5, 0xAB, 0xCC};  // E8:DB:84:0C:58:30

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

esp_now_peer_info_t peerInfo;

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  //Serial.print("\r\nLast Packet Send Status:\t");
  //Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_register_send_cb(OnDataSent);

  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
       
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
  pinMode(vrx, INPUT);
  pinMode(vry, INPUT);
  pinMode(sw, INPUT_PULLUP);
  pinMode(pot, INPUT);
  pinMode(fwd, INPUT_PULLUP);
  pinMode(rev, INPUT_PULLUP);
  pinMode(lef, INPUT_PULLUP);
  pinMode(rit, INPUT_PULLUP);
}

void loop() {

  if(digitalRead(sw) == 1){
    data.swi = 0;
  } else{
    data.swi = 1;
  }
  
  data.x = analogRead(vrx);
  data.y = analogRead(vry);
  data.front = digitalRead(fwd);
  data.back = digitalRead(rev);
  data.left = digitalRead(lef);
  data.right = digitalRead(rit);
  data.speed = map(analogRead(pot), 0, 4095, 0, 255);

  Serial.print(data.x);
  Serial.print("\t");
  Serial.print(data.y);
  Serial.print("\t");
  Serial.print(data.swi);
  Serial.print("\t");
  Serial.print(data.front);
  Serial.print("\t");
  Serial.print( data.back);
  Serial.print("\t");
  Serial.print(data.left);
  Serial.print("\t");
  Serial.print(data.right);
  Serial.print("\t");
  Serial.print(data.speed);
  Serial.println("\t");

  esp_now_send(broadcastAddress, (uint8_t *) &data, sizeof(data));
  delay(100);

}