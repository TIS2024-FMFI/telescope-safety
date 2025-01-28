#include <SPI.h>
#include <SD.h>
#include "logs.h"

const int _MISO = 4;
const int _MOSI = 7;
const int _CS = 5;
const int _SCK = 6;

const char *logConfigFileName = "Log_configuration.csv";
const char *logCollisionFileName = "Log_collisions.csv";


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

// Funkcia zapisuje AzimuthElevation do log súboru
// @param azimuthElevation - ukazovateľ na štruktúru AzimuthElevation
// @return 0, ak úspech, -1, ak chyba
int writeAEtoLog(AzimuthElevation *azimuthElevation) {
  Serial.println("Zapisujem log.");
  String logFileName = "Log-";
  logFileName += dateToString(getRealTime());
  logFileName += ".csv";
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
  return 0;
}

// Writes change of configuration to log file, with timestamp in CSV format
// @param changeType type of change
// @return 0 if success, -1 if error
int writeChangeToLog(ChangeType changeType, const char *ip) {
  if (writeHeaderIfNeeded(logConfigFileName, "Timestamp;ChangeType;IP;") != 0) {
    return -1;
  }

  File myFile = SD.open(logConfigFileName, FILE_WRITE);
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
  if (writeHeaderIfNeeded(logCollisionFileName, "Timestamp;Azimuth;Elevation;") != 0) {
    return -1;
  }

  File myFile = SD.open(logCollisionFileName, FILE_WRITE);
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

int writeConfigAlarmAndIntervals(const char* data) {
  const char* ConfigFileName = "AlarmAndIntervalsConfig.txt";
  File myFile = SD.open(ConfigFileName, O_WRITE | O_CREAT | O_TRUNC);
  if (!myFile) {
    return -1;
  }

  char* inputCopy = strdup(data);
  if (!inputCopy) {
    myFile.close();
    return -1;
  }

  char* audiovisual = strtok(inputCopy, ";");
  char* shutDown = strtok(NULL, ";");
  char* updateInterval = strtok(NULL, ";");
  char* logInterval = strtok(NULL, ";");
  char* logs = strtok(NULL, ";");

  myFile.print("Audiovizualne upozornenie:");
  myFile.println(audiovisual);
  
  myFile.print("Odpojenie systemu:");
  myFile.println(shutDown);

  myFile.print("Interval aktualizacii:");
  myFile.println(updateInterval);

  myFile.print("Interval logovania:");
  myFile.println(logInterval);

  myFile.print("Vypnut logovanie:");
  myFile.println(logs);

  free(inputCopy);

  myFile.close();
  return 0;
}

int writeNewForbiddenConfig(const char* zones) {
  const char* forbiddenConfigFileName = "MyZones.txt";
  File myFile = SD.open(forbiddenConfigFileName, O_WRITE | O_CREAT | O_TRUNC);
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

