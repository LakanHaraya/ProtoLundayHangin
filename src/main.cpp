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

struct PeriDevice {
    const char* name;
    const char* type;    // Bagong field para sa protocol (hal. "I2C")
    const char* model;
    uint8_t address;
    bool isWorking;
};

// Listahan ng mga I2C devices
PeriDevice periDevices[] = {
  // Mga Aparatong Periperal I2C 
  {"GPIOXL", "I2C", "MCP23017 I/O Expander Chip", 0x20, false},
  {"GPIOXR", "I2C", "MCP23017 I/O Expander Chip", 0x21, false},
  {"EEPROM", "I2C", "ATMHK218 EEPROM-RTC Module", 0x57, false},
  {"RTC", "I2C", "DS3231 Real-Time Clock Module", 0x68, false},
  {"IMU", "I2C", "MPU6050 Inertial Measurement Unit Module", 0x69, false},

  // Mga Aparatong Periperal SPI
  {"MSC", "SPI", "Micro SD Card Module", 10, false},
};
const int numDevices = sizeof(periDevices) / sizeof(periDevices[0]);

void printTimestamp() {
  DateTime now = rtc.now();  // Get current time from the RTC
  
  // Format the timestamp as [YY-MM-DD HH:MM:SS]
  char timestamp[20];
  snprintf(timestamp, sizeof(timestamp), "%02d-%02d-%02d %02d:%02d:%02d",
           now.year() - 2000, now.month(), now.day(),
           now.hour(), now.minute(), now.second());
  
  // Print the timestamp
  Serial.print("[");
  Serial.print(timestamp);
  Serial.print("] ");
}

void printLogMess(const char* type, const char* logMessage) {
  // Listahan ng mga pinapayagang log types
  const char* validTypes[] = {
    COMMS,  CTRL,   ERROR,  INIT,
    INFO,   LOG,    POWER,  RESET,
    SHUTD,  SNSOR,  WARN
  };
  bool isValidType = false;

  // Suriin kung ang 'type' ay isa sa mga valid types
  for (size_t i = 0; i < sizeof(validTypes) / sizeof(validTypes[0]); i++) {
    if (strcmp(type, validTypes[i]) == 0) {
      isValidType = true;
      break;
    }
  }

  // Mag-print lamang kung valid ang type at naka-enable ang PRINTLOGS
  if (isValidType && PRINTLOGS) {
    printTimestamp();
    Serial.print("[");
    Serial.print(type);
    Serial.print("] ");
    Serial.println(logMessage);
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

void checkPeriDevices() {
  for (int i = 0; i < numDevices; i++) {
    if (strcmp(periDevices[i].type, "I2C") == 0) {
      Wire.beginTransmission(periDevices[i].address);
      periDevices[i].isWorking = (Wire.endTransmission() == 0);
    } 
    else if (strcmp(periDevices[i].type, "SPI") == 0) {
      pinMode(periDevices[i].address, OUTPUT);
      digitalWrite(periDevices[i].address, LOW);

      SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
      uint8_t response = SPI.transfer(0xFF);  // Magpadala ng dummy byte
      SPI.endTransaction();
            
      digitalWrite(periDevices[i].address, HIGH);
            
      // Ang isang working SPI device ay karaniwang may *non-zero* response
      periDevices[i].isWorking = (response != 0xFF);
    }
  }
}

void printDeviceStatus() {
  bool allDevicesFound = true;

  // Loop sa lahat ng periDevices para i-check ang status
  for (int i = 0; i < numDevices; i++) {
    char buffer[100];  // Buffer para sa formatted message
        
    if (periDevices[i].isWorking) {
      // Kung ang device ay nadetect (working), iprint ng [IMPO] (INFO)
      snprintf(buffer, sizeof(buffer), "Gumagana ang %s, %s 0x%02X", periDevices[i].name, periDevices[i].type, periDevices[i].address);
      printDebugMess(buffer);  // Ipinapadala sa log
    } 
    else {
    // Kung ang device ay hindi nadetect (not working), iprint ng [WARN] (BABALA)
      snprintf(buffer, sizeof(buffer), "Di-gumagana ang %s, %s 0x%02X", periDevices[i].name, periDevices[i].type, periDevices[i].address);
      printLogMess(WARN, buffer);  // Ipinapadala sa log
      allDevicesFound = false;    // Itakda ang `allDevicesFound` sa false kung may hindi nadetect
    }
  }

  // Kung lahat ng devices ay nadetect, iprint ang tagumpay
  if (allDevicesFound) {
    printLogMess(INFO, "Matagumpay na natagpuan ang lahat ng aparato.");
  }
  else {
    printLogMess(WARN, "Pakitingnan ang mga koneksiyon.");
  }
}


void setup() {
  Serial.begin(BAUD_RATE);
  Serial.println();

  // Initialize the RTC module
  if (!rtc.begin()) {
    printLogMess(ERROR, "Hindi matagpuan ang RTC.");
    while (1);
  }

  // If the RTC lost power, set the time to the compilation time
  if (rtc.lostPower()) {
    printLogMess(WARN, "Ang RTC ay nawalan ng koryente, itinatakda ang oras...");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));  // Set time to the compilation time
  }

  printLogMess(INIT, "Ang sistema ay sinisimulan.");

  Wire.begin();
  SPI.begin();

  delay(1000);  // Hintay sandali bago magsimula ang pag-check

  checkPeriDevices();
  printDeviceStatus();

  printLogMess(INIT, "Natapos ang pagsisimula ng sistema.");
}

void loop() {
  printLogMess(INFO, "Tuloy ang silo.");
  checkPeriDevices();
  printDeviceStatus();
  delay(10000);
}
