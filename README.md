# Smart Locker System

## Overview

This project implements a Smart Locker System using the ESP8266 microcontroller. The system allows remote control of locker locks through MQTT (publish and subcribe) messages and uses an ultrasonic sensor to detect if the locker is occupied. The main features of this system include:

- **MQTT Integration**: Remotely control locker locks by subscribing to MQTT topics.
- **Non-Blocking Execution**: Uses non-blocking techniques to handle multiple tasks efficiently.
- **Ultrasonic Sensing**: Detects if the locker contains an item.

## Hardware Requirements

- ESP8266 microcontroller
- 4 locker locks
- HC-SR04 ultrasonic sensor
- WiFi connection

## Software Requirements

- Arduino IDE with ESP8266 board support
- PubSubClient library for MQTT
- Ticker library for non-blocking timing

## Setup Instructions

1. **Install the Required Libraries in Arduino IDE**:
   - PubSubClient
   - Ticker

2. **Configure WiFi and MQTT Server**:
   - Update the `ssid` and `password` variables with your WiFi credentials.
   - Set the `mqtt_server` to your MQTT broker's address.

3. **Upload the Code**:
   - Use the Arduino IDE to upload the provided code to the ESP8266 microcontroller.

## Code Explanation

- **WiFi Setup**: Connects to the specified WiFi network and handles reconnection if the connection is lost.
- **MQTT Setup**: Connects to the MQTT broker and subscribes to topics for controlling locker locks.
- **Lock Control**: Handles the state of four locker locks based on received MQTT messages.
- **Ultrasonic Sensor**: Checks the distance measured by the sensor at regular intervals and publishes a message if the locker is occupied.
- **Non-Blocking Execution**: Uses `millis()` to manage timing for various tasks without blocking the main loop, ensuring smooth operation.

## Usage

1. **Power on the ESP8266**: The device will connect to the configured WiFi network.
2. **MQTT Communication**: Send `on` or `off` messages to control the locker locks via MQTT topics (`rpi/locker1`, `rpi/locker2`, `rpi/locker3`, `rpi/locker4`).
3. **Monitor the Locker**: The system will automatically check the ultrasonic sensor and publish a message if an item is detected inside the locker.

## Example MQTT Messages

- To open locker 1: 
Topic: rpi/locker1
Message: on
- To close locker 1:
Topic: rpi/locker1
Message: off

## Troubleshooting

- Ensure that the WiFi credentials are correct and the device is within range.
- Verify the MQTT broker address and ensure it is running.
- Check the wiring connections for the locker locks and the ultrasonic sensor.

## License

This project is licensed under the MIT License. See the LICENSE file for more details.
