#include <Arduino.h>
#include <Wire.h>

#define BAUD_RATE 115200

// Enumerasyon para sa mga adres ng aparatong IIC
enum IICDevice {
  EEPROM = 0x57,
  RTC = 0x68,
  IMU = 0x69,
  GPIOXL = 0x20,
  GPIOXR = 0x21
};

// Estruktura para mag-imbak ng impormasyon ng aparato
struct IICDeviceInfo {
  IICDevice device;
  const char* name;
};

// Hanay ng mga aparato na gusto nating beripikahin
const IICDeviceInfo devices[] = {
  {GPIOXL, "GPIOXL (MCP23017)"},
  {GPIOXR, "GPIOXR (MCP23017)"},
  {EEPROM, "EEPROM (ATMHK218)"},
  {RTC, "RTC (DS3231)"},
  {IMU, "IMU (MPU6050)"}
};

// Funsiyon para beripikahin ang mga aparatong IIC
void checkIICDevices() {
  int detectedCount = 0;
  int errorCount = 0;

  Serial.println("\t_________________________________________________________________________");
  Serial.println("\t___________________________ MGA APARATONG IIC ___________________________");

  // Iterasyon sa bawat aparato
  for (size_t i = 0; i < sizeof(devices) / sizeof(devices[0]); i++) {
    byte iicaddr = static_cast<byte>(devices[i].device);
    Wire.beginTransmission(iicaddr);
    char output[80]; // Buffer para sa formatted string

    // Itransmit ang address at i-check ang resulta
    if (Wire.endTransmission() == 0) {
      detectedCount++;
      snprintf(output, sizeof(output), "\t\t0x%02X\t| %-20s | OK", iicaddr, devices[i].name);
    } else {
      errorCount++;
      snprintf(output, sizeof(output), "\t\t0x%02X\t| %-20s | ERROR (Hindi Nadetek)", iicaddr, devices[i].name);
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
  Wire.begin(); // Inisyalisahin ang bulos IIC
  Serial.begin(BAUD_RATE); // Simulan ang komunikasyong sunuran
  Serial.println("\n\tINISYALISASYON...\n");

  checkIICDevices(); // Tawagin ang function para beripikahin ang mga aparato

  Serial.println("\n\tNATAPOS ANG INISYALISASYON!");
}

void loop() {
  // Pwedeng magdagdag ng iba pang lohiko dito kung kinakailangan
}
