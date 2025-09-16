# Vehicle Accident Alert System Using ADXL-335 Sensor | GPS | GSM

[![Watch the video](https://img.youtube.com/vi/Bku3CtTWgqo/hqdefault.jpg)](https://youtu.be/Bku3CtTWgqo?si=RD02xtYHzjYiNUwO)

A smart **Vehicle Accident Alert System** using the ADXL-335 accelerometer sensor combined with GPS and GSM modules. This project detects sudden impacts indicating an accident, then automatically sends an alert SMS and places a call with the GPS location to a preset mobile number for emergency response.

---

## Project Overview

This system aims to enhance road safety by providing rapid accident alert notifications. When the ADXL-335 sensor detects an impact exceeding a set threshold, the microcontroller triggers the GSM module to call and send an SMS with the GPS coordinates (latitude and longitude) of the accident location.

---

## Components Required

- Arduino Nano Board  
- ADXL-335 Analog Accelerometer Sensor  
- SIM800L GSM Module  
- Neo-6M GPS Module  
- Push Buttons (2 units)  
- 10k Resistor  
- LM2596 Step Down Converter  
- Optional: 16x2 LCD Display with I2C Module  
- Zero PCB for mounting  
- USB Cable for programming  

---

## Wiring Summary

- GPS Module connected to 5V, RX/TX pins to Arduino (GPS TX → Arduino RX, GPS RX → Arduino TX)  
- SIM800L GSM module connected to 3.3–4V power with TX/RX pins to Arduino (TX → pin 2, RX → pin 3)  
- ADXL-335 accelerometer connected to analog pins A0, A1, and A2  
- Push buttons connected to digital pins 10 and 11  
- Optional LCD display connected via I2C  

---

## Features

- Detects sudden acceleration changes using ADXL-335 sensor  
- Sends an emergency call via SIM800L GSM module  
- Sends SMS with GPS coordinates from Neo-6M GPS module  
- Optional LCD to display crash detection and magnitude  
- Adjustable sensitivity threshold for impact detection  

---

## Installation & Setup

1. Connect components as per the wiring summary.  
2. Install required Arduino libraries:
   - LiquidCrystal_I2C  
   - TinyGPSPlus  
   - Math.h (standard)  
3. Configure mobile number in the source code for alerting.  
4. Upload the Arduino code to the Nano board.  
5. Power the system and test by simulating an impact on the ADXL-335 sensor.  

---

## Code and Circuit Diagram

Find full source code, detailed circuit diagrams, and step-by-step instructions on the project website:

[https://justdoelectronics.com/vehicle-accident-alert-system-using-adxl-335-sensor/](https://justdoelectronics.com/vehicle-accident-alert-system-using-adxl-335-sensor/)

---

## Video Demonstration

Watch the complete build and demonstration of the system here:

[![Watch the video](https://img.youtube.com/vi/Bku3CtTWgqo/hqdefault.jpg)](https://youtu.be/Bku3CtTWgqo?si=RD02xtYHzjYiNUwO)

<iframe width="560" height="315" src="https://www.youtube.com/embed/Bku3CtTWgqo?autoplay=1" frameborder="0" allow="autoplay; encrypted-media" allowfullscreen></iframe>

---


*This README is inspired by the Just Do Electronics Vehicle Accident Alert System video (May 28, 2024).*  
