#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <RCSwitch.h>

RCSwitch rfReceiver = RCSwitch();
LiquidCrystal_I2C lcd(0x27, 16, 2); 

// Appliance Control Pins
const int relay1 = 26;
const int relay2 = 25;
const int relay3 = 33;
const int relay4 = 32;

// Specific RF Codes
const long RF_CODE_1 = 7922369;
const long RF_CODE_2 = 7922370;
const long RF_CODE_3 = 7922372;
const long RF_CODE_4 = 7922376;

void setup() {
    Serial.begin(115200);
    rfReceiver.enableReceive(digitalPinToInterrupt(4)); // RF Receiver on GPIO4
    
    lcd.init();
    lcd.backlight();
    lcd.setCursor(0, 0);
    lcd.print("System Ready");
    
    pinMode(relay1, OUTPUT);
    pinMode(relay2, OUTPUT);
    pinMode(relay3, OUTPUT);
    pinMode(relay4, OUTPUT);
    
    digitalWrite(relay1, LOW);
    digitalWrite(relay2, LOW);
    digitalWrite(relay3, LOW);
    digitalWrite(relay4, LOW);
    
    delay(3000);
    lcd.clear();
    updateLCD();
}

void loop() {
    if (rfReceiver.available()) {
        long receivedData = rfReceiver.getReceivedValue();
        Serial.print("RF Signal Received: ");
        Serial.println(receivedData);
        
        if (receivedData == RF_CODE_1) {
            toggleRelay(relay1);
        } else if (receivedData == RF_CODE_2) {
            toggleRelay(relay2);
        } else if (receivedData == RF_CODE_3) {
            toggleRelay(relay3);
        } else if (receivedData == RF_CODE_4) {
            toggleRelay(relay4);
        } 
        rfReceiver.resetAvailable();
    }
}

void toggleRelay(int relayPin) {
    digitalWrite(relayPin, !digitalRead(relayPin));
    Serial.print("Relay ");
    Serial.print(relayPin);
    Serial.print(" Toggled! Status: ");
    Serial.println(digitalRead(relayPin) ? "ON" : "OFF");
    updateLCD();
}

void updateLCD() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("R1:"); lcd.print(digitalRead(relay1) ? "ON " : "OFF");
    lcd.print(" R2:"); lcd.print(digitalRead(relay2) ? "ON " : "OFF");
    lcd.setCursor(0, 1);
    lcd.print("R3:"); lcd.print(digitalRead(relay3) ? "ON " : "OFF");
    lcd.print(" R4:"); lcd.print(digitalRead(relay4) ? "ON " : "OFF");
}
