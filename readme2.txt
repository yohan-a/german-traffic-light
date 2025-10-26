# German Traffic Light Control System

A web-controlled, German-style traffic light simulator using ESP8266 hardware and a modern web interface. Simulates authentic German traffic light sequences, including the unique red+yellow transition phase.

## Table of Contents

- [Project Overview](#project-overview)
- [Hardware Requirements](#hardware-requirements)
- [Software Requirements](#software-requirements)
- [Installation](#installation)
- [Features](#features)
- [Usage](#usage)
- [Credits](#credits)
- [License](#license)

## Project Overview

This project simulates the behavior of a German traffic light system with accurate timing, transitions, and control. It includes an Arduino sketch for an ESP8266 microcontroller and a web-based interface for visualization and interaction.

## Hardware Requirements

- ESP8266 microcontroller
- 3 LEDs: red, yellow, green (connected to D1, D2, D3)
- 3 resistors for LEDs (typically 220Ω)
- Breadboard and jumper wires

## Software Requirements

- Arduino IDE with ESP8266 board support
- Libraries: `ESP8266WiFi`, `ESP8266WebServer`, `FS` (SPIFFS)

## Installation

1. **Clone the repository**
2. **Set WiFi credentials**
Open `traffic_light.ino` in the Arduino IDE and update your WiFi SSID and password.
3. **Upload the Sketch**
Upload `traffic_light.ino` to your ESP8266 board.
4. **Upload Web Interface to SPIFFS**
Use the Arduino IDE's “ESP8266 Sketch Data Upload” tool to upload `index.html` (and any assets) to SPIFFS.
5. **Start the System**
Open the Serial Monitor at 115200 baud to find the device’s IP address.
6. **Access the Web Interface**
Open the IP address shown in the Serial Monitor in your browser.

## Features

- Authentic German traffic light sequences (including red+yellow transition)
- RESTful API:
 - `/` Web Interface
 - `/state` GET current state
 - `/set` POST to change state
- Real-time synchronization (500ms polling)
- Non-blocking state logic with `millis()`
- Decorative interface: animated clouds, sun, trees, and road

## Usage

- Click light icons in the web interface to change states
- Automatic timing:
 - Yellow: 2000ms
 - Red+Yellow: 1500ms
- State and timing sync between board and browser in real time

## Credits

Developed by Yohan Amaratunga and Anirudh Gopishankar for Mechatronics Lab, August 2025.

## License

[Insert license here, e.g., MIT License]
