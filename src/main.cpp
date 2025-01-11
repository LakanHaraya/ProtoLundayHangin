#include <Arduino.h>
#include <Wire.h>

#define BAUD_RATE 115200

enum I2CDevice {
  EEPROM = 0x57,
  RTC = 0x68,
  IMU = 0x69,
  GPIOXL = 0x20,
  GPIOXR = 0x21,
  DI_KILALA = 0x00 
};

// Estruktura para mag-imbak ng impormasyon ng aparato
struct I2CDeviceInfo {
  I2CDevice device;
  byte address;
  const char* name;
};

// Hanay ng mga aparato na gusto mong beripikahin
I2CDeviceInfo devices[] = {
  {GPIOXL, 0x20, "GPIOXL (MCP23017)"},
  {GPIOXR, 0x21, "GPIOXR (MCP23017)"},
  {EEPROM, 0x57, "EEPROM (ATMHK218)"},
  {RTC, 0x68, "RTC (DS3231)"},
  {IMU, 0x69, "IMU (MPU6050)"}
};

void checkI2CDevices() {
  int detectedCount = 0;
  int errorCount = 0;
  Serial.println("\t_________________________________________________________________________");
  Serial.println("\t___________________________ MGA APARATONG I2C ___________________________");
  
  for (size_t i = 0; i < sizeof(devices) / sizeof(devices[0]); i++) {
    byte address = devices[i].address;
    Wire.beginTransmission(address);
    char output[80]; // Buffer para sa formatted string
    
    if (Wire.endTransmission() == 0) {
      detectedCount++;
      sprintf(output, "\t\t0x%02X\t[ %-20s ]\t- OK", address, devices[i].name);
    } else {
      errorCount++;
      sprintf(output, "\t\t0x%02X\t[ %-20s ]\t- ERROR (Hindi Nadetek)", address, devices[i].name);
    }
    Serial.println(output);
  }

  // Magpakita ng buod
  Serial.print("\n\t\tBUOD: \t\t Nadetek: ");
  Serial.print(detectedCount);
  Serial.print("\t\tHindi Nadetek: ");
  Serial.println(errorCount);
  Serial.println("\t_________________________________________________________________________");
}


void setup() {
  Wire.begin();
  Serial.begin(BAUD_RATE);
  Serial.println("\n\tINISYALISASYON...\n");

  checkI2CDevices();

  Serial.println("\n\tNATAPOS ANG INISYALISASYON!");
}

void loop() {
  
}