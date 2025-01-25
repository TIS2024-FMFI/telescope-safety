#include <SPI.h>
#include <SD.h>
#include "logs.h"

const int _MISO = 4;
const int _MOSI = 7;
const int _CS = 5;
const int _SCK = 6;

const char *logFileName = "";
const char *timestamp = "";
const char *logConfigFileName = "";
const char *logCollisionFileName = "";


void setupSD() {
  Serial.begin(9600);
  while (!Serial);

  Serial.print("Initializing SD card...");
  SPI.setRX(_MISO);
  SPI.setTX(_MOSI);
  SPI.setSCK(_SCK);

  if (!SD.begin(_CS)) {
    return;
  }
}

// Funkcia na kontrolu a zápis hlavičky do súboru
// @param filename - názov súboru
// @param header - hlavička, ktorá sa má zapísať
// @return 0, ak úspech, -1, ak chyba
int writeHeaderIfNeeded(const char *filename, const char *header) {
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
    if (writeHeaderIfNeeded(logFileName, "Timestamp;Azimuth;Elevation;") != 0) {
        return -1;
    }

    File myFile = SD.open(logFileName, FILE_WRITE);
    if (!myFile) {
        return -1;
    }

    myFile.print(timestamp);
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

    myFile.print(timestamp);
    myFile.print(";");

    switch (changeType) {
        case FORBIDDEN_ZONE_CHANGED:
            myFile.print("FORBIDDEN_ZONE_CHANGED");
            break;
        case LOG_FREQUENCY_CHANGED:
            myFile.print("LOG_FREQUENCY_CHANGED");
            break;
        case ALARM_TYPE_CHANGED:
            myFile.print("ALARM_TYPE_CHANGED");
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

    myFile.print(timestamp);
    myFile.print(";");
    myFile.print(azimuthElevation->azimuth, 2);
    myFile.print(";");
    myFile.print(azimuthElevation->elevation, 2);
    myFile.println(";");

    myFile.close();
    return 0;
}

// Checks if file format is correct
// @param newConfiguration from user
// @return 0 if correct, -1 if error
// @note Different error codes can be added
int checkFileFormat(char* newConfiguration) {
  char* current = newConfiguration;
  char* end = newConfiguration;
  int pointCount = 0;
  bool inZone = false;

  while (*current != '\0') {
    while (*end != '\0' && *end != '\n') {
      end++;
    }

  size_t length = end - current;
  char line[length + 1];
  strncpy(line, current, length);
  line[length] = '\0';

  if (*end == '\n') {
    end++;
  }
  current = end;

  if (line[0] == '#') {
    continue;
  }

  if (strlen(line) == 0) {
    if (inZone && pointCount < 3) {
      return -1;
    }
  pointCount = 0;
  inZone = false;
  } else {
    inZone = true;

    double az, el;
    if (sscanf(line, "%f %f", &az, &el) != 2) {
      return -1;
    }

    pointCount++;
    }
  }

  if (inZone && pointCount < 3) {
    return -1;
  }

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

int writeConfigZones(const char* zones) {
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


