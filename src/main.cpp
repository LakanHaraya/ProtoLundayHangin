#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <RTClib.h>

RTC_DS3231 rtc;

#define BAUD_RATE 115200
#define PRINTLOGS 1  // Palitan ng 0 kung gusto mong hindi paganahin ang log messages
#define PRINTDEBUGS 0

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

unsigned long previousMillis1 = 0; 
unsigned long previousMillis2 = 0;
const long interval1 = 5000;  // Interval para sa task ng `checkI2CDevices()` (5 segundo)
const long interval2 = 5000;  // Interval para sa task ng `checkSPIDevices()` (5 segundo)

// Estrukturang datos para sa mga periperal I2C
struct I2CPeripheral {
    const char* name;
    const char* model;
    uint8_t address;
    bool status;
};

// Estrukturang datos para sa mga periperal SPI
struct SPIPeripheral {
  const char* name;
  const char* model;
  uint8_t csPin;
  bool status;
};

// Listahan ng mga aparatong I2C 
I2CPeripheral i2cDevices[] = {
  {"GPIOXL", "MCP23017 I/O Expander Chip", 0x20, false},
  {"GPIOXR", "MCP23017 I/O Expander Chip", 0x21, false},
  {"EEPROM", "ATMHK218 EEPROM-RTC Module", 0x57, false},
  {"RTC", "DS3231 Real-Time Clock Module", 0x68, false},
  {"IMU", "MPU6050 Inertial Measurement Unit Module", 0x69, false}
};

// Listahan ng mga aparatong SPI
SPIPeripheral spiDevices[] = {
  {"MSC", "Micro SD Card Module", 10, false}
};

// Bilang ng mga aparato
const int numI2C = sizeof(i2cDevices) / sizeof(i2cDevices[0]);
const int numSPI = sizeof(spiDevices) / sizeof(spiDevices[0]);

// Funsiyon para sa paglimbag ng talabakasang mensahe
void printLogMess(const char* type, const char* logMessage) {
  // Listahan ng mga pinapayagang uri ng talabakasan
  const char* validTypes[] = {
    COMMS,  CTRL,   ERROR,  INIT,
    INFO,   LOG,    POWER,  RESET,
    SHUTD,  SNSOR,  WARN
  };
  bool isValidType = false;

  // Suriin kung ang 'type' ay isa sa mga tinatanggap na uri
  for (size_t i = 0; i < sizeof(validTypes) / sizeof(validTypes[0]); i++) {
    if (strcmp(type, validTypes[i]) == 0) {
      isValidType = true;
      break;
    }
  }

  // Maglimbag lamang kung tanggap ang uri at pinapagana ang PRINTLOGS
  if (isValidType && PRINTLOGS) {
    // printTimestamp();
    Serial.print("[");
    Serial.print(type);
    Serial.print("] ");
    Serial.println(logMessage);
  }
}

// Funsiyon para sa paglimbag ng dalisapang mensahe
void printDebugMess(const char* logMessage) {
  if (PRINTLOGS && PRINTDEBUGS) {
    // printTimestamp();
    Serial.print("[");
    Serial.print(DEBUG);
    Serial.print("] ");
    Serial.println(logMessage);
  }
}

// Function para mag-check ng koneksyon sa isang I2C device
bool checkI2CConnection(uint8_t address) {
  Wire.beginTransmission(address);
  byte error = Wire.endTransmission(); // Nagpapadala ng signal sa device

  if (error == 0) {
    // Kung walang error, nangangahulugang nakakonekta ang device
    return true;
  } else {
    // Kung may error, itinuturing na hindi nakakonekta ang device
    return false;
  }
}

// Function para i-check ang mga I2C devices
void checkI2CDevices() {
  for (int i = 0; i < numI2C; i++) {
    bool deviceStatus = checkI2CConnection(i2cDevices[i].address); // Mag-check ng koneksyon ng I2C device
    if (deviceStatus != i2cDevices[i].status) {
      // Kung may pagbabago sa status ng device (Online/Offline), i-update ang status
      i2cDevices[i].status = deviceStatus;
      if (deviceStatus) {
        Serial.print(i2cDevices[i].name);
        Serial.println(" is now ONLINE.");
      } else {
        Serial.print(i2cDevices[i].name);
        Serial.println(" is now OFFLINE.");
      }
    }
  }
}

// Function para mag-check ng koneksyon sa isang SPI device
bool checkSPIConnection(uint8_t csPin) {
  // Simulating a SPI communication test by trying to begin a transaction
  pinMode(csPin, OUTPUT);
  digitalWrite(csPin, LOW); // I-enable ang chip select
  SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
  
  // Check kung successful ang communication (may isang simpleng pagpapadala ng data)
  bool success = SPI.transfer(0x00) != 0x00; // Kung may response, ibig sabihin ay connected
  
  SPI.endTransaction();
  digitalWrite(csPin, HIGH); // I-disable ang chip select
  
  return success;
}

// Function para i-check ang mga SPI devices
void checkSPIDevices() {
  for (int i = 0; i < numSPI; i++) {
    bool deviceStatus = checkSPIConnection(spiDevices[i].csPin); // Mag-check ng koneksyon ng SPI device
    if (deviceStatus != spiDevices[i].status) {
      // Kung may pagbabago sa status ng device (Online/Offline), i-update ang status
      spiDevices[i].status = deviceStatus;
      if (deviceStatus) {
        Serial.print(spiDevices[i].name);
        Serial.println(" is now ONLINE.");
      } else {
        Serial.print(spiDevices[i].name);
        Serial.println(" is now OFFLINE.");
      }
    }
  }
}

// Funsiyon ng pagtatakda
void setup() {
  Serial.begin(BAUD_RATE);
  Serial.println();
  printLogMess(INIT, "Ang sistema ay sinisimulan.");

  Wire.begin(); // Simulan ang bulos I2C
  SPI.begin();  // Simulan ang bulos SPI

  checkI2CDevices();
  checkSPIDevices();

  delay(1000);  // Hintay sandali bago magsimula ang pag-check

  printLogMess(INIT, "Natapos ang pagsisimula ng sistema.");
}

// Funsiyon ng pinauulit
void loop() {
  unsigned long currentMillis = millis();  // Kunin ang kasalukuyang oras mula sa millis()

  // Task 1 (mag-check ng I2C devices tuwing 1 segundo)
  if (currentMillis - previousMillis1 >= interval1) {
    previousMillis1 = currentMillis;  // I-update ang oras ng huling pag-check
    checkI2CDevices();
  }

  // Task 2 (mag-check ng SPI devices tuwing 5 segundo)
  if (currentMillis - previousMillis2 >= interval2) {
    previousMillis2 = currentMillis;  // I-update ang oras ng huling pag-check
    checkSPIDevices();
  }

  // Ang iba pang mga task na walang time-based requirement ay patuloy na tatakbo
}
