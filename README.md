
# SensorDataOverHTTP

# Modbus and WiFi Integration with Arduino

This project integrates Modbus communication with an Arduino board and establishes a WiFi server to provide real-time sensor data over HTTP requests.

## Requirements
- Arduino board (tested with Arduino Uno)
- ESPAsyncWebServer library
- ModbusMaster library
- WiFi library
- SPI library (included with Arduino IDE)
- ModbusServerTCPasync library

## Components Used
- Temperature and Humidity sensor (Modbus device)
- ESP32 (or similar) for WiFi functionality

## Setup Instructions
1. Connect your Modbus device to the Arduino using appropriate connections (RS485, RS232, etc.).
2. Upload the provided code to your Arduino board using the Arduino IDE.
3. Adjust WiFi configuration (SSID, password, IP addresses) in the code as needed.
4. Verify connections and power up your Arduino board.

## Functionality
- **Modbus Communication:** Communicates with the Modbus device to read temperature and humidity values.
- **HTTP Server:** Provides endpoints ("/t" for temperature, "/h" for humidity) to fetch sensor data via HTTP GET requests.
- **Serial Output:** Outputs sensor readings and status messages to the Serial monitor for debugging.

## Usage
- Once deployed, connect to the WiFi network hosted by your Arduino (SSID: "3602236", password: "WiFi3602236").
- Access sensor data by making HTTP GET requests to `http://192.168.4.22/t` for temperature and `http://192.168.4.22/h` for humidity.

## Troubleshooting
- Ensure the Modbus device is properly configured and connected.
- Check Serial monitor output for any error messages or debugging information.
