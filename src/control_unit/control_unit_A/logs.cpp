#include <SPI.h>
#include <SD.h>
#include <SdFat.h>
#include <TimeLib.h>
#include "logs.h"

const int _MISO = 4;
const int _MOSI = 7;
const int _CS = 5;
const int _SCK = 6;

const char *logConfigFilePath = "/logs/events/Log_configuration.csv";
const char *logCollisionFilePath = "/logs/events/Log_collisions.csv";
const char *logFilePathPrefix = "/logs/Log_";
const char* ConfigFilePath = "/conf/AlarmAndIntervalsConfig.txt";
const char* forbiddenConfigFilePath = "/conf/zones.txt";
const char* matrixFilePath = "/conf/Matrix.txt";
SdFat mySdFat;


void setupSD() {
  Serial.print("Initializing SD card...");
  SPI.setRX(_MISO);
  SPI.setTX(_MOSI);
  SPI.setSCK(_SCK);

  if (!SD.begin(_CS)) {
    Serial.println("nie je dobre inicializovana SD");
    return;
  }

  if (!mySdFat.begin(_CS)) {
    Serial.println("Chyba inicializácie SdFat!");
    return;
  }
  Serial.println("SdFat inicializovaná.");
  Serial.println("jupi");
}

// Funkcia na kontrolu a zápis hlavičky do súboru
// @param filename - názov súboru
// @param header - hlavička, ktorá sa má zapísať
// @return 0, ak úspech, -1, ak chyba
int writeHeaderIfNeeded(String filename, const char *header) {
  if (!SD.exists(filename)) {
    File myFile = SD.open(filename, FILE_WRITE);
    if (!myFile) {
      return -1;
    }

    myFile.println(header);
    myFile.close();
  }
  return 0;
}

unsigned long lastLog = 0;
const int secendsToMilis = 1000;

// Funkcia zapisuje AzimuthElevation do log súboru
// @param azimuthElevation - ukazovateľ na štruktúru AzimuthElevation
// @return 0, ak úspech, -1, ak chyba
int writeAEtoLog(AzimuthElevation *azimuthElevation) {
  if ((!settings.logging) && millis() - lastLog >= (settings.log_frequency * secendsToMilis)){
    lastLog = millis();

    String logFileName = logFilePathPrefix;
    logFileName.concat(dateToString(getRealTime()));
    logFileName.concat(".csv");
    if (writeHeaderIfNeeded(logFileName, "Timestamp;Azimuth;Elevation;") != 0) {
      return -1;
    }

    File myFile = SD.open(logFileName, FILE_WRITE);
    if (!myFile) {
      return -1;
    }

    myFile.print(timeToString(getRealTime()));
    myFile.print(";");
    myFile.print(azimuthElevation->azimuth, 2);
    myFile.print(";");
    myFile.print(azimuthElevation->elevation, 2);
    myFile.println(";");

    myFile.close();
  }
  return 0;
}

// Writes change of configuration to log file, with timestamp in CSV format
// @param changeType type of change
// @return 0 if success, -1 if error
int writeChangeToLog(ChangeType changeType, const char *ip) {
  if (writeHeaderIfNeeded(logConfigFilePath, "Timestamp;ChangeType;IP;") != 0) {
    return -1;
  }

  File myFile = SD.open(logConfigFilePath, FILE_WRITE);
  if (!myFile) {
    return -1;
  }

  myFile.print(dateToString(getRealTime()));
  myFile.print("_");
  myFile.print(timeToString(getRealTime()));
  myFile.print(";");

  switch (changeType) {
    case FORBIDDEN_ZONE_CHANGED:
      myFile.print("FORBIDDEN_ZONE_CHANGED");
      break;
    case LOG_FREQUENCY_AND_ALARM_TYPE_CHANGED:
      myFile.print("LOG_FREQUENCY_AND_ALARM_TYPE_CHANGED");
      break;
    case TRANSFORM_MATRIX_CHANGED:
      myFile.print("TRANSFORM_MATRIX_CHANGED");
      break;
    case RESTART:
      myFile.print("RESTART");
      break;
    default:
      myFile.print("UNKNOWN_CHANGE");
      break;
  }
  myFile.print(";");
  myFile.print(ip);
  myFile.println(";");

  myFile.close();
  return 0;
}

// Writes alarm data when the telescope enters the forbidden zone
// @param azimuthElevation pointer to AzimuthElevation structure
// @return 0 if success, -1 if error
int writeAlarmToLog(AzimuthElevation *azimuthElevation) {
  if (writeHeaderIfNeeded(logCollisionFilePath, "Timestamp;Azimuth;Elevation;") != 0) {
    return -1;
  }

  File myFile = SD.open(logCollisionFilePath, FILE_WRITE);
  if (!myFile) {
    return -1;
  }

  myFile.print(dateToString(getRealTime()));
  myFile.print("_");
  myFile.print(timeToString(getRealTime()));
  myFile.print(";");
  myFile.print(azimuthElevation->azimuth, 2);
  myFile.print(";");
  myFile.print(azimuthElevation->elevation, 2);
  myFile.println(";");

  myFile.close();
  return 0;
}

int writeConfigAlarmAndIntervals(Settings settings) {
  File myFile = SD.open(ConfigFilePath, O_WRITE | O_CREAT | O_TRUNC);
  if (!myFile) {
    return -1;
  }

  myFile.print("Audiovizualne upozornenie:");
  myFile.println(settings.alarm);

  myFile.print("Odpojenie systemu:");
  myFile.println(settings.rele);

  myFile.print("Interval aktualizacii:");
  myFile.println(settings.update_frequency);

  myFile.print("Interval logovania:");
  myFile.println(settings.log_frequency);

  myFile.print("Vypnut logovanie:");
  myFile.println(settings.logging);

  myFile.close();
  return 0;
}

int writeNewForbiddenConfig(const char* zones) {
  File myFile = SD.open(forbiddenConfigFilePath, O_WRITE | O_CREAT | O_TRUNC);
  if (!myFile) {
    return -1;
  }
  myFile.print(zones);
  myFile.close();
  return 0;
}

char* loadFile(const char* filePath) {
  File myFile = SD.open(filePath, FILE_READ);
  if (!myFile) {
    return NULL; 
  }

  size_t fileSize = myFile.size();
  if (fileSize == 0) {
    myFile.close();
    return NULL;
  }

  char* fileContent = (char*)malloc(fileSize + 1);
  if (!fileContent) {
    myFile.close();
    Serial.println("Nepodarilo sa alokovat");
    return NULL;
  }

  myFile.read((uint8_t*)fileContent, fileSize);
  fileContent[fileSize] = '\0';
  myFile.close();

  return fileContent;
}

int loadSettings(){
  char* fileData = loadFile(forbiddenConfigFilePath);
  if (fileData) {
    int result = setUpZones(fileData);
    free(fileData);
    if(result!=0){
      Serial.println("Unexpected error while setting up the zones from file.");
      return -1;
    }
  }
  else{
    Serial.println("neexistuje subor");
    return -1;
  }
  fileData = loadFile(ConfigFilePath);
  if (fileData) {
    int result = setUpAlarmAndIntervals(fileData);
    free(fileData);
    if(result!=0){
      Serial.println("Unexpected error while setting up the alarm and intervals from file.");
      return -1;
    }
  }
  else{
    Serial.println("neexistuje subor");
    return -1;
  }
  Serial.println("uspech");
  return 0;
}

int saveMatrix() {
  File myFile = SD.open(matrixFilePath, O_WRITE | O_CREAT | O_TRUNC);
  if (!myFile) {
    return -1;
  }

  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      myFile.print(TransformMatrix[i][j]);
      if (j < 2) {
        myFile.print(";");
      }
    }
    myFile.println(); 
  }

  myFile.close();
  return 0;
}

// Pomocná funkcia: vráti voľné miesto na SD karte v bajtoch.
// Táto verzia používa metódy dostupné v niektorých verziách SD knižnice.
uint32_t getFreeSpace() {
  uint32_t freeClusters = mySdFat.vol()->freeClusterCount();
  uint32_t blocksPerCluster = mySdFat.vol()->sectorsPerCluster();
  // Predpokladáme veľkosť bloku 512 bajtov
  return freeClusters * blocksPerCluster * 512UL;
}

// Pomocná funkcia: zo súborového názvu (String) extrahuje dátum.
// Očakáva sa, že názov súboru obsahuje na začiatku napr. "Log_2023-03-14".
// Ak je extrakcia úspešná, hodnoty year, month a day sa nastavia a funkcia vráti true.
bool parseLogFileDate(const String &filename, int &year, int &month, int &day) {
  // Ak je názov súboru napr. "/logs/Log_2023-03-14_someInfo.csv",
  // odstránime cestu – hľadáme posledný znak '/'.
  int lastSlash = filename.lastIndexOf('/');
  String baseName = (lastSlash >= 0) ? filename.substring(lastSlash + 1) : filename;

  if (!baseName.startsWith("Log_")) {
    return false;
  }
  // Po "Log_" by mal nasledovať dátum vo formáte "YYYY-MM-DD"
  // Minimálna dĺžka: 4 (Log_) + 10 (YYYY-MM-DD) = 14 znakov.
  if (baseName.length() < 14) {
    return false;
  }
  // Extrahujeme 10 znakov začínajúcich na pozícii 4.
  String dateStr = baseName.substring(4, 14);  // pozície 4 až 13
  // Očakávaný formát je "YYYY-MM-DD"
  year = dateStr.substring(0, 4).toInt();
  month = dateStr.substring(5, 7).toInt();
  day = dateStr.substring(8, 10).toInt();
  return true;
}

// Pomocná funkcia: prevedie zadaný dátum (year, month, day) na Unix timestamp.
// Použije sa vzorec pre výpočet juliánskeho dňa a prepočet na sekundy.
unsigned long dateToTimestamp(int year, int month, int day) {
  int a = (14 - month) / 12;
  int y = year + 4800 - a;
  int m = month + 12 * a - 3;
  long julianDay = day + (153 * m + 2) / 5 + 365 * y + y / 4 - y / 100 + y / 400 - 32045;
  long daysSinceEpoch = julianDay - 2440588;  // 2440588 je juliánsky deň pre 1970-01-01
  return (unsigned long)daysSinceEpoch * 86400UL;  // počet sekúnd (86400 sekúnd za deň)
}

// Funkcia, ktorá prejde adresár "/logs" a vymaže všetky logovacie súbory,
// ktorých dátum (podľa názvu) je starší ako 6 mesiacov.
void deleteOldLogs() {
  // Definujeme 6 mesiacov ako 6 * 30 dní (približne 180 dní).
  const unsigned long SIX_MONTHS_SECONDS = 180UL * 86400UL;
  unsigned long currentTime = convertTimeToEpoch(getRealTime());
  unsigned long thresholdTime = currentTime - SIX_MONTHS_SECONDS;
  //unsigned long thresholdTime = 0;

  Serial.print("Threshold (6 mesiacov dozadu) timestamp: ");
  Serial.println(thresholdTime);

  // Otvoríme adresár "/logs"
  File logsDir = SD.open("/logs");
  if (!logsDir) {
    Serial.println("Chyba: nepodarilo sa otvoriť adresár /logs.");
    return;
  }

  // Prejdeme všetky súbory v adresári
  while (true) {
    File entry = logsDir.openNextFile();
    if (!entry) break;  // koniec zoznamu
    if (!entry.isDirectory()) {
      String fileName = entry.name();
      int year, month, day;
      if (parseLogFileDate(fileName, year, month, day)) {
        unsigned long fileTimestamp = dateToTimestamp(year, month, day);
        Serial.print("Súbor: ");
        Serial.print(fileName);
        Serial.print(" má timestamp: ");
        Serial.println(fileTimestamp);
        
        // Ak je dátum súboru starší (menší timestamp) ako threshold, vymažeme ho.
        if (fileTimestamp <= thresholdTime) {
          String fullPath = "/logs/";
          fullPath += fileName;
          SD.remove(fullPath);
          Serial.print("Vymazaný súbor: ");
          Serial.println(fileName);
        }
      }
    }
    entry.close();
  }
  logsDir.close();
}

// Funkcia, ktorá skontroluje voľné miesto na SD karte a ak je pod stanoveným prahom,
// vymaže logovacie súbory staršie ako 6 mesiacov.
void manageSDSpace() {
  const uint32_t MIN_FREE_BYTES = 1000UL * 1024UL;  // napr. 1000 kB
  uint32_t freeBytes = getFreeSpace();
  Serial.print("Voľné miesto: ");
  Serial.print(freeBytes);
  Serial.println(" bajtov");
  Serial.println("MAZEEEEEM");
  deleteOldLogs();

  if (freeBytes < MIN_FREE_BYTES) {
    Serial.println("Nedostatok miesta – mažeme logy staršie ako 6 mesiacov.");
    deleteOldLogs();
    freeBytes = getFreeSpace();
    Serial.print("Voľné miesto po mazaní: ");
    Serial.print(freeBytes);
    Serial.println(" bajtov");
  }
}

unsigned long convertTimeToEpoch(const Time &t) {
  tmElements_t tm;
  // TimeLib.h očakáva, že pole Year obsahuje počet rokov od 1970.
  tm.Year = t.year - 1970;  // napr. ak je t.year = 2023, tm.Year bude 53
  tm.Month = t.month;
  tm.Day = t.day;
  tm.Hour = t.hours;
  tm.Minute = t.minutes;
  tm.Second = t.seconds;
  return makeTime(tm);  // Vracia time_t, ktorý je unsigned long
}