#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <RTClib.h>

RTC_DS3231 rtc;
Sd2Card card;
SdVolume volume;
File root;

#define BAUD_RATE 115200
#define PRINTLOGS 1  // Palitan ng 0 kung gusto mong hindi paganahin ang log messages
#define PRINTDEBUGS 1

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

struct I2CPeripheral {
    const char* name;
    uint8_t address;
    bool status;
};

struct SPIPeripheral {
    const char* name;
    uint8_t csPin;
    bool status;
};

I2CPeripheral i2cDevices[] = {
    {"IOXL", 0x20, false},
    {"IOXR", 0x21, false},
    {"ERC", 0x57, false},
    {"RTC", 0x68, false},
    {"IMU", 0x69, false}
};

SPIPeripheral spiDevices[] = {
    {"MSC", 10, false}
};

const int numI2C = sizeof(i2cDevices) / sizeof(i2cDevices[0]);
const int numSPI = sizeof(spiDevices) / sizeof(spiDevices[0]);

unsigned long prevMillisTask01 = 0;
unsigned long prevMillisTask02 = 0;
const long intervalTask01 = 5000;
const long intervalTask02 = 5000;

void printTimestamp() {
  DateTime now = rtc.now();
  char timestamp[21];
  snprintf(timestamp, sizeof(timestamp), "[%02d-%02d-%02d %02d:%02d:%02d] ", now.year() % 100, now.month(), now.day(), now.hour(), now.minute(), now.second());
  Serial.print(timestamp);
}

void printLogMess(const char* type, const char* logMessage) {
  const char* validTypes[] = {
    COMMS, CTRL, ERROR, INIT, INFO, LOG, POWER, RESET, SHUTD, SNSOR, WARN
  };
  bool isValidType = false;
  for (size_t i = 0; i < sizeof(validTypes) / sizeof(validTypes[0]); i++) {
    if (strcmp(type, validTypes[i]) == 0) { isValidType = true; break; }
  }
  if (isValidType && PRINTLOGS) {
    char logBuffer[100];
    printTimestamp();
    snprintf(logBuffer, sizeof(logBuffer), "[%s] %s", type, logMessage);
    Serial.println(logBuffer);
  }
}

void printDebugMess(const char* logMessage) {
  if (PRINTLOGS && PRINTDEBUGS) {
    printTimestamp();
    Serial.print("[");
    Serial.print(DEBUG);
    Serial.print("] ");
    Serial.println(logMessage);
  }
}

void printDeviStatus(const char* name, bool status) {
  printLogMess(INFO, ("Ang " + String(name) + " ngayon ay " + String(status ? "SUMASALINYA." : "DI-SUMASALINYA.")).c_str());
}

bool checkI2CConnection(uint8_t address) {
  Wire.beginTransmission(address);
  return (Wire.endTransmission() == 0);
}

bool checkSPIConnection(uint8_t csPin) {
  pinMode(csPin, OUTPUT);
  digitalWrite(csPin, LOW);
  SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
  bool success = SPI.transfer(0x00) != 0x00;
  SPI.endTransaction();
  digitalWrite(csPin, HIGH);
  return success;
}

void checkI2CDevices(bool initialCheck) {
  for (int i = 0; i < numI2C; i++) {
    bool newStatus = checkI2CConnection(i2cDevices[i].address);
    if (initialCheck || newStatus != i2cDevices[i].status) {
      i2cDevices[i].status = newStatus;
      printDeviStatus(i2cDevices[i].name, newStatus);
    }
  }
}

void checkSPIDevices(bool initialCheck) {
  for (int i = 0; i < numSPI; i++) {
    bool newStatus = checkSPIConnection(spiDevices[i].csPin);
    if (initialCheck || newStatus != spiDevices[i].status) {
      spiDevices[i].status = newStatus;
      printDeviStatus(spiDevices[i].name, newStatus);
    }
  }
}

bool sdCardInfoPrinted = false;
void checkSDCardDetails() {
  if (sdCardInfoPrinted) return;
  if (!card.init(SPI_HALF_SPEED, spiDevices[0].csPin)) {
    printLogMess(ERROR, "Hindi ma-initialize ang SD Card.");
    return;
  }
  if (!volume.init(card)) {
    printLogMess(ERROR, "Hindi mabasa ang filesystem ng SD Card.");
    return;
  }
  uint32_t blockSize = 512; // Karaniwang block size
  uint32_t clusterSize = volume.blocksPerCluster() * blockSize;
  uint32_t totalBlocks = volume.blocksPerCluster() * volume.clusterCount();
  uint32_t totalSizeMB = (uint64_t)totalBlocks * blockSize / 1024 / 1024;

  printLogMess(INFO, (String("Laki ng SD Card: ") + totalSizeMB + " MB").c_str());
  printLogMess(INFO, (String("File System: ") + (volume.fatType() == 32 ? "FAT32" : "FAT16")).c_str());
  printLogMess(INFO, (String("Kabuuang Bloke: ") + totalBlocks).c_str());
  printLogMess(INFO, (String("Laki ng Cluster: ") + clusterSize + " bytes").c_str());

  sdCardInfoPrinted = true;
}

void checkSDCard(bool initialCheck) {
  static bool sdCardStatus = false;
  bool newStatus = SD.begin(spiDevices[0].csPin);
  if (initialCheck || newStatus != sdCardStatus) {
    sdCardStatus = newStatus;
    printLogMess(INFO, newStatus ? "Ang SD Kard ay SUMASALINYA." : "Ang SD Kard ay DI-SUMASALINYA.");
  }
  if (newStatus) {
    checkSDCardDetails();
  }
}

void setup() {
  Serial.begin(BAUD_RATE);
  Wire.begin();
  SPI.begin();
  Serial.println();
  if (!rtc.begin()) {
    printLogMess(ERROR, "Hindi matagpuan ang RTC.");
    while (1) {
      printLogMess(INFO, "Pakitingnan ang mga koneksiyon.");
    };
  }
  if (rtc.lostPower()) {
    printLogMess(WARN, "Nawalan ng koryente ang RTC, muling tinatakda ang oras.");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
  printLogMess(INIT, "Inisyalisasyon ng Sistema.");
  checkI2CDevices(true);
  checkSPIDevices(true);
  checkSDCard(true);
  printLogMess(INIT, "Natapos ang Inisyalisasyon.");
}

void loop() {
  unsigned long currentMillis = millis();
    
  if (currentMillis - prevMillisTask01 >= intervalTask01) {
    prevMillisTask01 = currentMillis;
    checkI2CDevices(false);
    checkSPIDevices(false);
  }
    
  if (currentMillis - prevMillisTask02 >= intervalTask02) {
    prevMillisTask02 = currentMillis;
    checkSDCard(false);
  }
}
