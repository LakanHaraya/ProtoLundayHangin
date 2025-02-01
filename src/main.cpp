// Kumusta Pabs!
// Wala pang 'checkSPIPeripheral()', baka gusto mong simulan na!
//
// 'checkIICPeripheral()'
// 'printPeripheralStat()'
// 'debugPrint()'
// 
// 

#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>

#define BAUD_RATE 115200
#define DEBUG 0  // Palitan ng 0 kung gusto mong hindi paganahin ang debug messages

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

/**
 * @brief  Ipinapakita ang debug message sa Serial Monitor.
 * 
 * @param debugTitle   Ang pamagat o kategorya ng debug message.
 * @param debugMessage Ang mismong mensahe na ipapakita.
 */
void debugPrint(String debugTitle, String debugMessage) {
  if (DEBUG) { // Itsek kung pinagana ang debugging
    Serial.print("DEBUGGING: \n");
    Serial.print("\t\'" + debugTitle + "\'\n");
    Serial.print("\t" + debugMessage + "\n");
    Serial.println();
  }
}

/**
 * @brief  Ipinapakita ang estado ng lahat ng nakakonektang periperal sa isang talahanayan.
 * 
 * @details Ipinapadala sa Serial Monitor ang isang naka-format na talahanayan
 * na naglalaman ng impormasyon tungkol sa bawat periperal, kasama ang kanilang
 * pangalan, protokol, adres, mikrotsip, at kasalukuyang estado.
 * 
 * @note   Tiyaking napunan nang tama ang `peripherals` at `peripheralStatus` bago ito tawagin.
 */
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
  Serial.println("\t+--------+----------+-----------+----------+--------+\n");
}

/**
 * @brief  Sinusuri kung may tugon mula sa isang I2C periperal sa ibinigay na address.
 * 
 * @param  address  Ang I2C address ng periperal na susuriin.
 * 
 * @return `true` kung may tumugon na periperal, `false` kung wala.
 * 
 * @note   Gumagamit ito ng `Wire` library para sa komunikasyon sa I2C bus.
 */
bool scanIICPeripheral(uint8_t address) {
  Wire.beginTransmission(address);
  return Wire.endTransmission() == 0;  // Ang 0 ay nangangahulugang matagumpay
}

/**
 * @brief  Sinusuri ang koneksiyon ng lahat ng I2C periperal at isinasapanahon ang kanilang estado.
 * 
 * @details Ang funsiyon na ito ay dumadaan sa listahan ng mga periperal at tinitingnan kung 
 *          ang protokol nila ay "I2C". Kung gayon, susuriin ang kanilang address upang 
 *          matukoy kung nakakonekta sila. Ang resulta ay inilalagay sa `peripheralStatus[]` 
 *          bilang "OK" kung nakakonekta, at "ERROR" kung hindi.
 * 
 * @note   Gumagamit ito ng `scanIICPeripheral()` upang suriin ang bawat I2C device.
 */
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
  SPI.begin();
  Serial.begin(BAUD_RATE);
  Serial.println("INISYALISASYON...\n");

  debugPrint("checkIICPeripheral()", "Sisimulang magtsek ng mga IIC periperal.");
  checkIICPeripheral();
  debugPrint("checkIICPeripheral()", "Natapos ang pagtsek ng mga IIC periperal.");

  // Magdagdag ng 'checkSPIPeripheral()' dito! 
  
  debugPrint("printPeripheralStat()", "Sisimulang ipakita ang katayuan ng mga periperal.");
  printPeripheralStat();
  debugPrint("printPeripheralStat()", "Natapos ipakita ang katayuan ng mga periperal.");

  Serial.println("\nNatapos ang Inisyalisasyon.\n");
}

void loop() {
  debugPrint("ISINASAAYOS ANG MAYSILO...", "Huwag lumampas sa dilaw na linya.");
  delay(60000);
}
