/*

  RS485_HalfDuplex.pde - example using ModbusMaster library to communicate
  with EPSolar LS2024B controller using a half-duplex RS485 transceiver.

  This example is tested against an EPSolar LS2024B solar charge controller.
  See here for protocol specs:
  http://www.solar-elektro.cz/data/dokumenty/1733_modbus_protocol.pdf

  Library:: ModbusMaster
  Author:: Marius Kintel <marius at kintel dot net>

  Copyright:: 2009-2016 Doc Walker

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

*/

#include <ModbusMaster.h>

/*!
  We're using a MAX485-compatible RS485 Transceiver.
  Rx/Tx is hooked up to the hardware serial port at 'Serial'.
  The Data Enable and Receiver Enable pins are hooked up as follows:
*/
unsigned constexpr MAX485_DE = 3;
unsigned constexpr MAX485_RE_NEG = 2;
unsigned constexpr MODBUS_TIMEOUT = 30; //Timeout of Modbus response in ms
// instantiate ModbusMaster object
ModbusMaster node;

void preTransmission()
{
  digitalWrite(MAX485_RE_NEG, 1);
  digitalWrite(MAX485_DE, 1);
}

void postTransmission()
{
  digitalWrite(MAX485_RE_NEG, 0);
  digitalWrite(MAX485_DE, 0);
}

void setup()
{
  pinMode(MAX485_RE_NEG, OUTPUT);
  pinMode(MAX485_DE, OUTPUT);
  // Init in receive mode
  digitalWrite(MAX485_RE_NEG, 0);
  digitalWrite(MAX485_DE, 0);

  // Modbus communication runs at 115200 baud
  Serial.begin(115200);

  // Modbus slave ID 1
  node.begin(1, Serial);
  // Callbacks allow us to configure the RS485 transceiver correctly
  node.preTransmission(preTransmission);
  node.postTransmission(postTransmission);
  node.setTimeout(MODBUS_TIMEOUT);
}



void loop()
{
  constexpr uint16_t register_addr = 0x3100;
  constexpr uint16_t total_registers = 0x16;
  //Read 16 Registers starting from Input register address 0x3100
  const uint8_t modbusOperation_status = node.readInputRegisters(register_addr, total_registers); 
  if (modbusOperation_status == node.ku8MBSuccess) //If modbusOperation_status == 0, got Data succesfully from Modbus Device 
  { 
    constexpr uint8_t battery_voltage_offset = 0x04; //register for battery voltage, starting offset of address 0x3100
    constexpr uint8_t voltage_load_offset = 0xC0; //register for voltage load, starting offset of address 0x3100
    float batteryVoltage = node.getResponseBuffer(battery_voltage_offset) / 100.0f; //Get data from 
    float VoltageLoad    = node.getResponseBuffer(voltage_load_offset) / 100.0f; //Get data from modbus device from register 0xC0 with starting offset of address 0x3100
    Serial.print("Vbatt: ");
    Serial.println(batteryVoltage);
    Serial.print("Vload: ");
    Serial.println(VoltageLoad);
  }

  Serial.println("Part 2");
  while (Serial2.available()) {
    Serial.print(char(Serial2.read()));

  delay(1000);
}
