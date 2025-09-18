# Arduino-Based Toll Collection System 🚗💳

An automated toll collection system using Arduino and RFID technology to enable seamless, contactless toll payments, reduce congestion, and improve traffic flow at toll plazas.

---

## 📖 About

This project employs an **Arduino Uno** microcontroller integrated with an **RFID reader (RC522)**, RFID tags/cards, a **servo motor** for gate control, **IR sensors** for vehicle detection, a **16x2 I2C LCD display** for status messages, and a **4x4 keypad** for manual recharge input. The system automatically identifies vehicles via RFID, processes toll payments, and controls the toll gate accordingly.

---

## ⚙️ Features

- **Automatic vehicle identification** using RFID tags/cards  
- **Real-time toll payment processing** with Arduino Uno  
- **Servo motor controlled gate** for opening/closing based on payment status  
- **IR sensors** to detect vehicle presence and trigger the process  
- **16x2 I2C LCD display** shows payment status (Access Granted / Access Denied)  
- **4x4 Keypad** for entering recharge amounts when balance is low  
- Organized wiring using a **PCB perfboard** for power distribution  

---

## 🛠️ Components Required

- Arduino Uno R3  
- RFID Reader Module (RC522)  
- RFID Tags/Cards  
- Servo Motor  
- IR Sensors  
- 16x2 I2C LCD Display  
- 4x4 Keypad  
- PCB perfboard for power distribution  
- Connecting wires and power supply  

---

## 🚀 How It Works

1. **Vehicle Detection:** IR sensor detects vehicle approaching the toll gate.  
2. **RFID Scan:** RFID reader scans the vehicle's RFID tag.  
3. **Verification:** Arduino checks the tag data and balance.  
4. **Gate Control:**  
   - If balance is sufficient, servo motor opens the gate; LCD shows "Access Granted."  
   - If balance is low or tag is invalid, gate remains closed; LCD shows "Access Denied."  
5. **Recharge:** Users can recharge their RFID card balance using the keypad if needed.  
6. **Traffic Flow:** After successful payment, the gate closes automatically after a delay, allowing smooth traffic flow.

---

## 📁 Repository Contents

- `Arduino_Based_Toll_Collection_System.ino` — Main Arduino code for the system  
- `RFID_Toll_Code` — Folder containing RFID-related code and libraries  
- Circuit diagram image: `Tollecollection Based Circuit Diagram.png`  
- Additional resources and keypad library (`OnewireKeypad-master.zip`)  

---

## 🔧 Setup Instructions

1. **Hardware:** Connect all components as per the circuit diagram provided.  
2. **Software:**  
   - Open the `.ino` file in Arduino IDE.  
   - Install required libraries (`MFRC522`, `Servo`, `LiquidCrystal_I2C`, etc.).  
3. **Upload:** Select the Arduino Uno board and appropriate COM port, then upload the code.  
4. **Test:** Present RFID cards to the reader and observe gate operation and LCD messages. Use the keypad to recharge cards as needed.

---

## 💡 Future Improvements

- Integrate a database for logging toll transactions and vehicle data.  
- Add mobile app support for balance checking and payments.  
- Enhance security with camera integration for license plate recognition.  
- Implement cloud connectivity for remote monitoring and analytics.

---

Made with ❤️ for smarter and efficient toll management! 🚦

