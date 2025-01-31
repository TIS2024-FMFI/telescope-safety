#include <SPI.h>
#include <SD.h>
#include "logs.h"

const int _MISO = 4;
const int _MOSI = 7;
const int _CS = 5;
const int _SCK = 6;

const char *logConfigFilePath = "/logs/events/Log_configuration.csv";
const char *logCollisionFilePath = "/logs/events/Log_collisions.csv";
const char *logFilePathPrefix = "/logs/Log-";
const char* ConfigFilePath = "/conf/AlarmAndIntervalsConfig.txt";
const char* forbiddenConfigFilePath = "/conf/zones.txt";


void setupSD() {
  Serial.print("Initializing SD card...");
  SPI.setRX(_MISO);
  SPI.setTX(_MOSI);
  SPI.setSCK(_SCK);

  if (!SD.begin(_CS)) {
    Serial.println("nie je dobre inicializovana SD");
    return;
  }
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
  if (settings.logging && millis() - lastLog >= (settings.log_frequency * secendsToMilis)){
    lastLog = millis();

    Serial.println("Zapisujem log.");
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

  myFile.print(timeToString(getRealTime()));
  myFile.print(";");

  switch (changeType) {
    case FORBIDDEN_ZONE_CHANGED:
      myFile.print("FORBIDDEN_ZONE_CHANGED");
      break;
    case LOG_FREQUENCY_AND_ALARM_TYPE_CHANGED:
      myFile.print("LOG_FREQUENCY_AND_ALARM_TYPE_CHANGED");
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
    return -1;
  }
  return 0;
}