#include <Arduino.h>
#include <ModbusMaster.h>
#include "WiFi.h"
#include "ESPAsyncWebServer.h"
#include "ModbusServerTCPasync.h"

unsigned constexpr MODBUS_TIMEOUT = 30; //Timeout of Modbus response in ms
// instantiate ModbusMaster object
ModbusMaster node;
constexpr uint16_t register_addr = 0x0000;
constexpr uint16_t total_registers = 0x02;
//Read 16 Registers starting from Input register address 0x3100

#define RXD2 16
#define TXD2 17

const char* ssid = "3602236";
const char* password = "WiFi3602236";
String s3, s;
int input;

ModbusServerTCPasync MBserver;

AsyncWebServer server(80);

IPAddress local_IP(192,168,4,22);
IPAddress gateway(192,168,4,9);
IPAddress subnet(255,255,255,0);

ModbusMessage FC03(ModbusMessage request) {
ModbusMessage response;      // The Modbus message we are going to give back
uint16_t addr = 0;           // Start address
uint16_t words = 0;          // # of words requested
request.get(2, addr);        // read address from request
request.get(4, words);       // read # of words from request

  response.add(request.getServerID(), request.getFunctionCode(), (uint8_t)(words * 2));
      // No, this is for FC 0x04. Response is random
   for (uint8_t i = 0; i < words; ++i) {
        // send increasing data values
        response.add((uint16_t)random(1, 65535));
      }
    
  return response;
  }

void setup()
{
  // Modbus communication runs at 115200 baud

  // Modbus slave ID 1
  // Callbacks allow us to configure the RS485 transceiver correctly

  node.setTimeout(MODBUS_TIMEOUT);

  Serial.println(WiFi.localIP());

  Serial.print("Setting soft-AP configuration ... ");
  Serial.println(WiFi.softAPConfig(local_IP, gateway, subnet) ? "Ready" : "Failed!");

  Serial.print("Setting soft-AP ... ");
  Serial.println(WiFi.softAP(ssid,password) ? "Ready" : "Failed!");
  
  Serial.print("Soft-AP IP address = ");
  Serial.println(WiFi.softAPIP());

  server.on(
    "/t", HTTP_GET, [](AsyncWebServerRequest *request){

    Serial.begin(115200);
    Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
    Serial.println("Serial Txd is on pin: "+String(TX));
    Serial.println("Serial Rxd is on pin: "+String(RX)); 
    node.begin(1, Serial2);
    const uint8_t modbusOperation_status = node.readInputRegisters(register_addr, total_registers);
    if (modbusOperation_status == node.ku8MBSuccess) //If modbusOperation_status == 0, got Data succesfully from Modbus Device 
  { 
    
    constexpr uint8_t Temp_offset = 0x00; //register for battery voltage, starting offset of address 0x3100
    //constexpr uint8_t Humidity_offset = 0x01; //register for voltage load, starting offset of address 0x3100
    float Temperature = node.getResponseBuffer(Temp_offset) / 10.0f; //Get data from 
    //float Humidity    = node.getResponseBuffer(Humidity_offset) / 10.0f; //Get data from modbus device from register 0xC0 with starting offset of address 0x3100
    
    Serial.print("Temperature: ");
    Serial.println(Temperature);
    s = (String) Temperature;
  }
    request->send(200,"text/plain", s);
  });

  server.on(
    "/h", HTTP_GET, [](AsyncWebServerRequest *request){
    Serial.begin(115200);
    Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
    Serial.println("Serial Txd is on pin: "+String(TX));
    Serial.println("Serial Rxd is on pin: "+String(RX));
    node.begin(1, Serial2);
    const uint8_t modbusOperation_status = node.readInputRegisters(register_addr, total_registers);
      if (modbusOperation_status == node.ku8MBSuccess) //If modbusOperation_status == 0, got Data succesfully from Modbus Device 
  { 
    //constexpr uint8_t Temp_offset = 0x00; //register for battery voltage, starting offset of address 0x3100
    constexpr uint8_t Humidity_offset = 0x01; //register for voltage load, starting offset of address 0x3100
    //float Temperature = node.getResponseBuffer(Temp_offset) / 10.0f; //Get data from 
    float Humidity    = node.getResponseBuffer(Humidity_offset) / 10.0f; //Get data from modbus device from register 0xC0 with starting offset of address 0x3100
    
    Serial.print("Humidity: ");
    Serial.println(Humidity);
    s = (String) Humidity;
  }
    request->send(200, "text/plain", s);
  });

  MBserver.registerWorker(1, READ_HOLD_REGISTER, &FC03);      // FC=03 for serverID=1
  
  MBserver.registerWorker(1, READ_INPUT_REGISTER, &FC03);     // FC=04 for serverID=1
  MBserver.start(502, 1, 20000);

  server.begin();

}


void loop()
{
  
}
