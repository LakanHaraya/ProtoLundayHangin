#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <RTClib.h>

RTC_DS3231 rtc;

#define BAUD_RATE 115200
#define PRINTLOGS 1  // Palitan ng 0 kung gusto mong hindi paganahin ang log messages
#define PRINTDEBUGS 0

#define CSP_MSC 10    // chip select pin ng micro sd card module

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

// Funsiyon para sa pagtatatak ng oras
// void printTimestamp() {
//   DateTime now = rtc.now();  // Kumuha ng kasalukuyang oras mula sa RTC
  
//   // Iformat ang tatakang-oras bilang [YY-MM-DD HH:MM:SS]
//   char timestamp[20];
//   snprintf(timestamp, sizeof(timestamp), "%02d-%02d-%02d %02d:%02d:%02d", now.year() - 2000, now.month(), now.day(), now.hour(), now.minute(), now.second());
  
//   // Ilimbag ang tatakang-oras
//   Serial.print("[");
//   Serial.print(timestamp);
//   Serial.print("] ");
// }

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

// Funsiyon para sa pagtsetsek ng aparatong I2C 
void checkI2CDevices() {
  for (int i = 0; i < numI2C; i++) {
    Wire.beginTransmission(i2cDevices[i].address);
    if (Wire.endTransmission() == 0) {
      i2cDevices[i].status = true;  // Natagpuan ang aparato
    }
    else {
      i2cDevices[i].status = false; // Di-konektadong aparato
    }
  }
}

// Funsiyon para sa pagtsetsek ng mga aparatong SPI
void checkSPIDevices() {
  for (int i = 0; i < numSPI; i++) {
    digitalWrite(spiDevices[i].csPin, LOW);  // Simulan ang komunikasyon
    delay(10);  // Bigyan ng kaunting oras para magsimula ang device
    byte response = 0;
    
    // Magpadala ng isang byte (example: 0x01) at hintayin ang sagot
    SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));  // Set SPI settings
    response = SPI.transfer(0x01);  // Magpadala ng data at tumanggap ng sagot
    SPI.endTransaction();

    // Kung walang natanggap na tamang response, hindi connected
    if (response == 0xFF) {
      spiDevices[i].status = false;
      printLogMess(ERROR, ("SPI device " + String(spiDevices[i].name) + " hindi nakakonect.").c_str());
    }
    else {
      spiDevices[i].status = true;
      printLogMess(INFO, ("SPI device " + String(spiDevices[i].name) + " ay nakakonect.").c_str());
    }

    digitalWrite(spiDevices[i].csPin, HIGH);  // Tapusin ang komunikasyon
  }
}

// Funsiyon ng pagtatakda
void setup() {
  Serial.begin(BAUD_RATE);
  Serial.println();
  printLogMess(INIT, "Ang sistema ay sinisimulan.");

  // // Inisyalisahin ang modyul RTC
  // if (!rtc.begin()) {
  //   printLogMess(ERROR, "Hindi matagpuan ang RTC.");
  //   while (1);
  // }
  // // Kung nawalan ng koryente ang RTC, itakda ang oras sa oras ng kompilasyon
  // if (rtc.lostPower()) {
  //   printLogMess(WARN, "Ang RTC ay nawalan ng koryente, itinatakda ang oras...");
  //   rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));  // Itakda ang oras sa oras ng kompilasyon
  // }

  Wire.begin(); // Simulan ang bulos I2C
  SPI.begin();  // Simulan ang bulos SPI

  delay(1000);  // Hintay sandali bago magsimula ang pag-check

  for (int i = 0; i < numSPI; i++) {
    pinMode(spiDevices[i].csPin, OUTPUT);
  }

  printLogMess(INIT, "Natapos ang pagsisimula ng sistema.");
}

// Funsiyon ng pinauulit
void loop() {
  printLogMess(INFO, "Tuloy ang silo.");
  checkI2CDevices();
  checkSPIDevices();
  Serial.println("== I2C Device Status ==");
  for (int i = 0; i < numI2C; i++) {
      Serial.print(i2cDevices[i].name);
      Serial.print(" (0x");
      Serial.print(i2cDevices[i].address, HEX);
      Serial.print("): ");
      Serial.println(i2cDevices[i].status ? "SUMASALINYA" : "DI-SUMASALINYA");
  }

  Serial.println("== SPI Device Status ==");
  for (int i = 0; i < numSPI; i++) {
      Serial.print(spiDevices[i].name);
      Serial.print(" (CS Pin ");
      Serial.print(spiDevices[i].csPin);
      Serial.print("): ");
      Serial.println(spiDevices[i].status ? "SUMASALINYA" : "DI-SUMASALINYA");
  }
  delay(10000);
}
