# Assignment 1 - ESP32 LED Mode cycling with OLED Display 
**Name:** Rameen Fatima  **Reg. No:** 23-NTU-CS-1086  
**Course:** Embedded IoT Systems  **Section:** BSCS - 5th B  

---

## Wokwi Project Link
(https://wokwi.com/projects/445351775582333953)

---

## Description
This project demonstrates how to use **ESP32** with **3 LEDs**, **2 push buttons**, and an **OLED display** to control different LED operation modes.  
The OLED screen shows the current mode, and the buttons allow the user to cycle through or reset modes.  

### LED Modes:
1. **Mode 0:** All LEDs OFF  
2. **Mode 1:** Alternate Blinking LEDs  
3. **Mode 2:** All LEDs ON  
4. **Mode 3:** PWM Fade (Cross-fade brightness effect)

---

## Pin Map

| Component      | ESP32 Pin  | Description |
|----------------|------------|--------------|
| LED 1          | 25         | First LED (PWM CH 0) |
| LED 2          | 26         | Second LED (PWM CH 1) |
| LED 3          | 33         | Third LED (PWM CH 2) |
| Mode Button    | 14         | Changes LED Mode |
| Reset Button   | 27         | Resets to OFF Mode |
| OLED SDA       | 21         | I²C Data Line |
| OLED SCL       | 22         | I²C Clock Line |
| OLED VCC       | 3.3V       | Power Supply |
| OLED GND       | GND        | Common Ground |

---

## Screenshots

### Wokwi Circuit
![Mode 1: Alternate Blinking](screenshots/Mode-1(Alternate_blinking).png)
![Mode 2: All ON](screenshots/Mode2(ALL-ON).png)
![Mode 3: PWM Fade](screenshots/Mode3(PWM-fade).png)

### Breadboard Diagram
![OLED Output](screenshots/Circuit_Mode2(ALL-ON).jpg)
![OLED Output](screenshots/Circuit_Mode3(PWM-fade).jpg)

More detailed screenshots are available in the `/screenshots` folder.

---

## Notes
- The complete source code is available in the `/src` folder.  
- Documentation and pin mapping are stored in the `/docs` folder.  
- Wokwi project JSON file is available inside the `/diagram.json` file and `/wokwi.toml` .   
---
