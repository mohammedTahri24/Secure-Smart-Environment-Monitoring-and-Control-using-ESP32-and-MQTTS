# 🔐 Secure Smart Environment Monitoring and Control using ESP32 and MQTTS

![ESP32](https://img.shields.io/badge/ESP32-IoT-blue)
![MQTT](https://img.shields.io/badge/MQTT-MQTTS-green)
![Node-RED](https://img.shields.io/badge/Node--RED-Dashboard-red)
![PlatformIO](https://img.shields.io/badge/PlatformIO-Embedded-orange)
![Wokwi](https://img.shields.io/badge/Wokwi-Simulation-purple)

A secure IoT-based environment monitoring and control system built with **ESP32**, **MQTT/MQTTS**, **Wokwi simulation**, and **Node-RED Dashboard**.

The system enables **real-time monitoring**, **remote control**, **alert management**, and **secure communication** using the Publish/Subscribe architecture.

---

# 📌 Project Overview

This project simulates an intelligent environment monitoring system capable of:

✅ Monitoring temperature and humidity in real time  
✅ Detecting abnormal environmental conditions  
✅ Triggering visual and sound alerts  
✅ Controlling devices remotely  
✅ Displaying telemetry in a dashboard  
✅ Securing communication using MQTT over TLS (MQTTS)

---

# 🏗 System Architecture

```text
VS Code + PlatformIO
        ↓
Firmware Compilation
        ↓
Wokwi Simulation (ESP32)
        ↓
MQTT / MQTTS
        ↓
broker.emqx.io
        ↓
Node-RED
        ↓
Dashboard
```

---

# ⚙️ Components

| Component | Role |
|---|---|
| ESP32 | Main controller |
| DHT22 | Temperature & Humidity Sensor |
| Servo Motor | Fan simulation |
| LED | Visual alert |
| Buzzer | Sound alarm |
| Push Buttons | Local controls |

---

# 📡 MQTT Communication

The system follows the **Publish / Subscribe** model.

## MQTT Broker

```text
broker.emqx.io
```

## Topics

| Topic | Direction | Description |
|---|---|---|
| `warehouse/esp32-01/telemetry` | ESP32 → Node-RED | Sensor data |
| `warehouse/esp32-01/commands` | Node-RED → ESP32 | Remote commands |
| `warehouse/esp32-01/events` | ESP32 → Node-RED | Alert events |

---

# 🎛 Dashboard Features

The Node-RED dashboard provides:

- 🌡 Temperature Monitoring
- 💧 Humidity Monitoring
- 🚨 Alert Detection
- 🌀 Fan Control
- 🔇 Alarm Mute
- 🔄 Alert Reset

Dashboard URL:

```text
http://localhost:1880/ui
```

---

# 🔐 Security Improvements

The communication layer can operate in:

## Standard MQTT

```text
Port 1883
```

## Secure MQTT (MQTTS)

```text
Port 8883
```

Security enhancements:

✔ TLS encryption  
✔ Secure communication  
✔ Reduced interception risk  

Implementation:

```cpp
WiFiClientSecure espClient;

espClient.setInsecure();
```

> For production environments, certificate validation should replace `setInsecure()`.

---

# 🧠 Project Workflow

## Monitoring

```text
ESP32
 ↓
Read DHT22
 ↓
Publish Telemetry
 ↓
Broker
 ↓
Node-RED Dashboard
```

## Remote Control

```text
Dashboard
 ↓
Send Command
 ↓
Broker
 ↓
ESP32
 ↓
Execute Action
```

---

# 🛠 Technologies Used

- ESP32
- PlatformIO
- C++
- MQTT / MQTTS
- EMQX Broker
- Wokwi
- Node-RED
- Node-RED Dashboard

---

# 🚀 Installation

## Clone Repository

```bash
git clone https://github.com/YOUR_USERNAME/YOUR_REPO.git
```

## Open Project

```bash
VS Code → PlatformIO
```

## Build

```bash
Ctrl + Alt + B
```

## Start Simulation

```text
Start Wokwi Simulation
```

## Launch Node-RED

```bash
node-red
```

Open:

```text
http://localhost:1880/ui
```

---

# 📷 Screenshots

## Wokwi Simulation

(Add screenshot)

---

## Node-RED Flow

(Add screenshot)

---

## Dashboard

(Add screenshot)

---

# 📈 Future Improvements

- Certificate validation
- Private MQTT Broker
- Historical Data Storage
- Cloud Integration
- User Authentication

---

# 👨‍💻 Authors

Developed as an IoT academic project.

---

# 📄 License

Educational Use