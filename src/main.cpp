#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>

#define BAUD_RATE 115200

const int spicsp = 10;  // serial peripheral interface chip select pin

// Estruktura para mag-imbak ng impormasyon ng aparato
struct PeripheralInfo {
  const char* name;     // Pangalan ng aparato
  const char* protocol; // Ginagamit na protokol
  const char* address;  // IIC address o kaya SPI chip select
  const char* chip;     // Uri ng tsip
};

// Hanay ng mga aparato na gusto nating isakodigo (hardcode)
const PeripheralInfo peripherals[] = {
  {"GPIOXL", "I2C", "0x20", "MCP23017"},
  {"GPIOXR", "I2C", "0x21", "MCP23017"},
  {"EEPROM", "I2C", "0x57", "ATMHK218"},
  {"RTC", "I2C", "0x68", "DS3231"},
  {"IMU", "I2C", "0x69", "MPU6050"},
  {"MSC", "SPI", "0x21-GPA7", "AIPA347"}
};

const size_t peripheralCount = sizeof(peripherals) / sizeof(peripherals[0]);

// Hanay ng estado para sa bawat peripheral (pwedeng baguhin)
const char* peripheralStatus[] = {
  "Wala", "Wala", "Wala", "Wala", "Wala", "Wala"
};

void printPeripheralStat() {
  // Pamuhatan ng talahanayan
  Serial.println("\t+---------------------------------------------------+");
  Serial.println("\t|                   MGA PERIPERAL                   |");
  Serial.println("\t+--------+----------+-----------+----------+--------+");
  Serial.println("\t| Ngalan | Protokol |   Adres   |   Tsip   | Estado |");
  Serial.println("\t+--------+----------+-----------+----------+--------+");

  // Ipakita ang mga periperal sa format ng talahanayan
  for (size_t i = 0; i < peripheralCount; i++) {
    char line[100];
    snprintf(
      line, sizeof(line),
      "\t| %-6s | %-8s | %-9s | %-8s | %-6s |",
      peripherals[i].name,
      peripherals[i].protocol,
      peripherals[i].address,
      peripherals[i].chip,
      peripheralStatus[i]  // I-print ang estado mula sa auxiliary array
    );
    Serial.println(line);
  }
  Serial.println("\t+--------+----------+-----------+----------+--------+");
}

bool scanIICPeripheral(uint8_t address) {
  Wire.beginTransmission(address);
  return Wire.endTransmission() == 0;  // Ang 0 ay nangangahulugang matagumpay
}

void checkIICPeripheral() {
  for (size_t i = 0; i < peripheralCount; i++) {
    if (strcmp(peripherals[i].protocol, "I2C") == 0) {
      uint8_t address = strtol(peripherals[i].address, NULL, 16);
      bool isConnected = scanIICPeripheral(address);

      // I-update ang estado sa auxiliary array
      if (isConnected) {
        peripheralStatus[i] = "OK";  // Baguhin ang estado sa auxiliary array
      } else {
        peripheralStatus[i] = "ERROR"; 
      }
    }
  }
}

void setup() {
  Wire.begin();
  Serial.begin(BAUD_RATE);
  Serial.println("INISYALISASYON...\n");

  checkIICPeripheral();

  

  printPeripheralStat();

  Serial.println("\nNatapos ang Inisyalisasyon.");
}

void loop() {
  // Walang ginagawa sa loop() sa ngayon
}
