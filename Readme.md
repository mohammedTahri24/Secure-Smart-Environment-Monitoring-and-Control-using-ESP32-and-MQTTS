# IoT Warehouse Monitoring System

This project is an IoT monitoring and control system based on ESP32, MQTT, Wokwi simulation, and Node-RED dashboard.

## Features

- Temperature and humidity monitoring using DHT22
- MQTT communication with EMQX public broker
- Real-time Node-RED dashboard
- Fan control using servo motor
- Alarm system using LED and buzzer
- Remote commands using MQTT topics

## Technologies Used

- ESP32
- PlatformIO
- Wokwi Simulator
- MQTT
- EMQX Public Broker
- Node-RED
- Node-RED Dashboard

## MQTT Topics

| Topic | Direction | Description |
|---|---|---|
| `warehouse/esp32-01/telemetry` | ESP32 → Node-RED | Sends temperature, humidity and system state |
| `warehouse/esp32-01/commands` | Node-RED → ESP32 | Sends commands to ESP32 |
| `warehouse/esp32-01/events` | ESP32 → Node-RED | Sends alert events |

## Dashboard

The Node-RED dashboard displays:

- Temperature gauge
- Humidity gauge
- Alert status
- Fan state
- Alarm mute state
- Control buttons

## Security

The project can use MQTT over TLS by switching from port `1883` to port `8883`.

## How to Run

1. Open the project in VS Code with PlatformIO.
2. Build the project.
3. Start the Wokwi simulation.
4. Open Node-RED.
5. Connect Node-RED to the MQTT broker.
6. Open the dashboard at:

```text
http://localhost:1880/ui