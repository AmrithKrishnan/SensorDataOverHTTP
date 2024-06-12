#include <Arduino.h>
#include <ModbusMaster.h>
#include "WiFi.h"
#include "ESPAsyncWebServer.h"
#include "ModbusServerTCPasync.h"
#include <spi.h>

unsigned constexpr MODBUS_TIMEOUT = 30;
ModbusMaster node;
constexpr uint16_t register_addr = 0x0000;
constexpr uint16_t total_registers = 0x02;
#define RXD2 16
#define TXD2 17
const char* ssid = "3602236";
const char* password = "WiFi3602236";
String s3, s;
int input;
uint16_t TEMP;
uint16_t HUM;
float Temperature;
float Humidity;

ModbusServerTCPasync MBserver;
AsyncWebServer server(80);
IPAddress local_IP(192,168,4,22);
IPAddress gateway(192,168,4,9);
IPAddress subnet(255,255,255,0);

void GetModbusSensorValues() // Gets the Modbus sensor values and stores them in TEMP, HUM, Temperature and Humidity 
                             // Values stored in four global variables in uint16_t and float data types
{
const uint8_t modbusOperation_status = node.readInputRegisters(register_addr, total_registers);
    if (modbusOperation_status == node.ku8MBSuccess) //If modbusOperation_status == 0, got Data succesfully from Modbus Device 
  { 
    
    constexpr uint8_t Temp_offset = 0x00; //register for Temp, starting offset of address 0x00
    constexpr uint8_t Humidity_offset = 0x01; //register for Hum, starting offset of address 0x00
    Temperature = node.getResponseBuffer(Temp_offset) / 10.0f; //Get data from modbus device
    Humidity    = node.getResponseBuffer(Humidity_offset) / 10.0f; //Get data from modbus device

    TEMP = Temperature;
    HUM = Humidity;
  }
}

ModbusMessage FC04(ModbusMessage request) // to build the response for TCP
{
  GetModbusSensorValues();

  ModbusMessage response;      // The Modbus message we are going to give back
  uint16_t addr = 0;           // Start address
  uint16_t words = 2;          // # of words requested
  request.get(2, addr);        // read address from request
  request.get(4, words);       // read # of words from request

  // Address overflow?
  if ((addr + words) > 20) {
    // Yes - send respective error response
    response.setError(request.getServerID(), request.getFunctionCode(), ILLEGAL_DATA_ADDRESS);
  }
  // Set up response
  response.add(request.getServerID(), request.getFunctionCode(), (uint8_t)(words * 2));
  response.add(TEMP);
  response.add(HUM);
  
  return response;
}

void HTTPrequest() // To initialize wifi and for HTTP requests
{
  //First set up the wifi
  Serial.begin(115200);
  Serial.println(WiFi.localIP());

  Serial.print("Setting soft-AP configuration ... ");
  Serial.println(WiFi.softAPConfig(local_IP, gateway, subnet) ? "Ready" : "Failed!");

  Serial.print("Setting soft-AP ... ");
  Serial.println(WiFi.softAP(ssid,password) ? "Ready" : "Failed!");
  
  Serial.print("Soft-AP IP address = ");
  Serial.println(WiFi.softAPIP());

  // Server on based on required HTTP request

  server.on( // /t for getting temperature data
    "/t", HTTP_GET, [](AsyncWebServerRequest *request){
    const uint8_t modbusOperation_status = node.readInputRegisters(register_addr, total_registers);
    if (modbusOperation_status == node.ku8MBSuccess) //If modbusOperation_status == 0, got Data succesfully from Modbus Device 
    { 
      GetModbusSensorValues(); 
      Serial.print("Temperature: ");
      Serial.println(Temperature);
      s = (String) Temperature; // typecast to String to display in the message
    }
    request->send(200,"text/plain", s);
  });

  server.on(  // /h for getting humidity data
    "/h", HTTP_GET, [](AsyncWebServerRequest *request){
    const uint8_t modbusOperation_status = node.readInputRegisters(register_addr, total_registers);
    if (modbusOperation_status == node.ku8MBSuccess) //If modbusOperation_status == 0, got Data succesfully from Modbus Device 
    { 
      GetModbusSensorValues();
      Serial.print("Humidity: ");
      Serial.println(Humidity);
      s = (String) Humidity;
    }
    request->send(200, "text/plain", s);
  });

  server.begin();
}

void ModbusMasterTCP()
{
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
  node.begin(1, Serial2);

  Serial.println("Serial Txd is on pin: "+String(TX));
  Serial.println("Serial Rxd is on pin: "+String(RX)); 
  
  node.setTimeout(MODBUS_TIMEOUT);

  MBserver.registerWorker(1, READ_INPUT_REGISTER, &FC04); // FC03 is the function name
  MBserver.start(502, 1, 20000);
}

void setup()
{
  HTTPrequest();       // function call for HTTP aspect of the task
  ModbusMasterTCP();   // function call for the TCP aspect 
}

void loop()
{
  
}
