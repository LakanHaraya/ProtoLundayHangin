#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>

#define BAUD_RATE 115200
#define PRINTLOGS 1  // Palitan ng 0 kung gusto mong hindi paganahin ang log messages

#define DEBUG "DALI"  // DALISAP
#define INFO "IMPO"   // IMPORMASYON
#define LOG "TBKS"    // TALABAKASAN
#define INIT "INIS"   // INISYALISASYON
#define RESET "ULIT"  // ULITMULI
#define SHUTD "SARA"  // PAGSARA
#define POWER "LKAS"  // LAKAS KORYENTE
#define COMMS "KOMU"  // KOMUNIKASYON
#define SNSOR "DAMA"  // PANDAMA
#define ERROR "MALI"  // KAMALIAN
#define CTRL "KTRL"   // KONTROL
#define WARN "BBLA"   // BABALA

struct I2CDevice {
    const char* name;
    const char* type;    // Bagong field para sa protocol (hal. "I2C")
    const char* model;
    uint8_t address;
    bool isWorking;
};

// Listahan ng mga I2C devices
I2CDevice devices[] = {
    {"GPIOXL", "I2C", "MCP23017", 0x20, false},
    {"GPIOXR", "I2C", "MCP23017", 0x21, false},
    {"EEPROM", "I2C", "ATMHK218", 0x57, false},
    {"RTC", "I2C", "DS3231", 0x68, false},
    {"IMU", "I2C", "MPU6050", 0x69, false}
};
const int numDevices = sizeof(devices) / sizeof(devices[0]);


void logMessage(const char* type, const char* message) {
  if (PRINTLOGS) {
    Serial.print("[");
    Serial.print(type);
    Serial.print("] ");
    Serial.println(message);
  }
}

void checkI2CDevices() {
  logMessage(INIT, "Pagsusuri ng mga nakakabit na I2C device...");

  char buffer[80];  // Pinalaki ang buffer para sa buong linya ng output
  for (int i = 0; i < numDevices; i++) { 
    Wire.beginTransmission(devices[i].address);
    uint8_t error = Wire.endTransmission();

    if (error == 0) {
      snprintf(buffer, sizeof(buffer), "Matatag ang %s (%s), Protokol: %s, Adres: 0x%02X", devices[i].name, devices[i].model, devices[i].type, devices[i].address);
      logMessage(INFO, buffer);

      devices[i].isWorking = true;  // I-update ang status
    }
    else {
      snprintf(buffer, sizeof(buffer), "Di-matatag ang %s (%s), Protokol: %s, Adres: 0x%02X", devices[i].name, devices[i].model, devices[i].type, devices[i].address);
      logMessage(WARN, buffer);
      devices[i].isWorking = false;
    }
  }
}


void setup() {
  Serial.begin(BAUD_RATE);
  Serial.println();
  logMessage(INIT, "Ang sistema ay sinisimulan.");
  Wire.begin();
  delay(1000);  // Hintay sandali bago magsimula ang pag-check

  checkI2CDevices();
  logMessage(INIT, "Natapos ang pagsisimula ng sistema.");
}

void loop() {
  logMessage(INFO, "Tuloy ang silo.");
  delay(120000);
}
