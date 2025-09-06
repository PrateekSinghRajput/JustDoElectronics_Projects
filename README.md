# 🌾 Controlling or Monitoring AC Power

## 📞 GSM-Based 3-Phase Motor Starter Using DTMF

### 🚀 Overview

This project automates farm irrigation by allowing farmers to control 3-phase AC motors remotely using **GSM** and **DTMF** technology.  
With this system, daily travel to crop fields to operate irrigation pumps can be replaced by simple phone calls, making irrigation more efficient and cost-effective for farmers. 🚜💧

### ✨ Features

- 📱 Remote motor control using GSM SIM modules and DTMF signals  
- 🛠️ Arduino-based C++ firmware for easy hardware integration  
- ⚡ Reliable switching of 3-phase motor starters for AC power  
- 🌐 No Internet needed—works entirely through mobile phone calls and keypad tones  

### 📋 Table of Contents

- [Overview](#-overview)  
- [Features](#-features)  
- [Hardware Requirements](#-hardware-requirements)  
- [Installation](#-installation)  
- [Usage](#-usage)  
- [Contribution](#-contribution)  
- [License](#-license)  

### 🔧 Hardware Requirements

- Arduino or compatible microcontroller  
- SIM800L/SIM900 GSM module  
- DTMF decoder module  
- Relay board for 3-phase starter control  
- 3-phase motor setup  

### 💻 Installation

1. Assemble the hardware according to the diagrams and `docs/`.  
2. Upload the provided Arduino C++ firmware from the `code/` folder using Arduino IDE.  
3. Insert an active SIM card into the GSM module and power everything on.  
4. Connect relay outputs to the motor starter circuits.

### 📞 Usage  

- Call the SIM number installed in the GSM module from your phone.  
- Use DTMF keypad:  
  - Press **'1'** to **start** the irrigation motor ▶️  
  - Press **'0'** to **stop** the motor ⏹️  
- The microcontroller receives the DTMF signal and controls the relay to switch the AC power.


---

🙏 Thank you for helping automate farming and making irrigation smarter!  

