# 🔥 SPI Temperature & Humidity Control System

This project implements a **Temperature & Humidity monitoring and control system** using:

- Arduino **Master + Slave** (SPI)
- **4-pin I²C LCD (PCF8574)**
- **Threshold control buttons**
- **Hardware interrupts**
- **Heater output**
- **Register-level SPI (no Arduino SPI library)**

The Master reads measurements from the Slave, displays them on the LCD, and activates a heater when temperature drops below a configurable threshold.

---

## 🧩 System Architecture

### **MASTER (Controller)**
- Requests temperature & humidity from SPI Slave
- Displays values on **I²C LCD**
- Buttons adjust the temperature threshold (`INT0` / `INT1`)
- Controls a heater/LED on pin **D7**
- Uses **register-level SPI**

### **SLAVE (Sensor Module)**
- Reads temperature & humidity from a sensor (e.g. DHT11/LM35)
- Responds to SPI commands:
  - `0x01` → temperature
  - `0x02` → humidity

---

## 🛠 Hardware Required

| Component | Description |
|----------|-------------|
| Arduino Uno / ATmega328P | Master |
| Arduino Uno / ATmega328P | Slave |
| 16×2 I²C LCD (PCF8574) | 4-pin LCD |
| Heater or LED | Controlled via D7 |
| 2× Momentary buttons | Threshold adjust |
| Temperature & humidity sensor | On slave |

---

## 🔌 Wiring

### **I²C LCD (4 pins) → Master**
| LCD Pin | Arduino Pin |
|---------|-------------|
| VCC     | 5V          |
| GND     | GND         |
| SDA     | A4          |
| SCL     | A5          |

---

### **SPI (Master → Slave)**

| Signal | Master Pin | Slave Pin |
|--------|-------------|------------|
| MOSI   | 11          | 11         |
| MISO   | 12          | 12         |
| SCK    | 13          | 13         |
| SS     | 10          | 10         |

---

### **Buttons (Master)**

| Button | Arduino Pin | Function |
|--------|--------------|----------|
| BTN_UP | D2 (INT0)    | Increase threshold |
| BTN_DOWN | D3 (INT1)  | Decrease threshold |

---

### **Heater Output**

- **Pin D7** controls heater (or LED/relay)

---
