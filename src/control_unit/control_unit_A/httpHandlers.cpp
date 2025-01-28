#include "httpHandlers.h"
#include <Arduino.h>
#include "logs.h"
#include <SD.h>
#include "forbidden_zones_config_parse.h"

#define DEBUG 1
String mainHTML;
String mainJS;
String CSS;
String confJS;
String confHTML;

int restart();



void setupStaticFiles(){
  mainHTML = loadFile("/www/index.html");
  confHTML = loadFile("/www/config.html");
  CSS = loadFile("/www/style.html");
  mainJS = loadFile("/www/main.js");
  confJS = loadFile("/www/config.js");
}


// Functions
void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";

  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void handleFileDownload() {
  String fileName = server.arg("file");
  if (fileName.length() == 0) {
    server.send(400, "text/plain", "File name not specified!");
    return;
  }

  File file = SD.open("/" + fileName);
  if (!file) {
    server.send(404, "text/plain", "File not found!");
    return;
  }

  server.streamFile(file, "text/csv");

  file.close();
}




void handleFormPOST() {
  #if DEBUG
  String message = "";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  Serial.println(message);
  #endif

  const char *clientIP = server.client().remoteIP().toString().c_str();

  if (server.arg(ZONE_CONFIG_BUTTON)){
    const char* newZones = server.arg(ZONE_CONFIG_FIELD).c_str();
    if (checkFileFormat(newZones) && writeNewForbiddenConfig(newZones)){
      writeChangeToLog(FORBIDDEN_ZONE_CHANGED, clientIP);
    }
  }
  else if (server.arg(ALARM_CONFIG_BUTTON)){
    String write = server.arg(LOG_INTERVAL_FIELD);
    write.concat(";");
    write.concat(server.arg(UPDATE_INTERVAL_FIELD));
    write.concat(";");
    if (server.arg(ALARM_CHECKBOX)){
      write.concat("1");
    }
    else{
      write.concat("0");
    }
    write.concat(";");
    if(server.arg(MOTORS_CHECKBOX)){
      write.concat("1");
    }
    else{
      write.concat("0");
    }
    write.concat(";");
    if(server.arg(TURN_OFF_LOGS_CHECKBOX)){
      write.concat("1");
    }
    else{
      write.concat("0");
    }
    if (writeConfigAlarmAndIntervals(write.c_str())){
      writeChangeToLog(LOG_FREQUENCY_AND_ALARM_TYPE_CHANGED, clientIP);
    }

  }
  else if (server.arg(RESTART_BUTTON)){
    writeChangeToLog(RESTART, clientIP);
    restart();
  }
  handleFormPage();
}



void handleFormPage(){
  // This needs some more work
  server.send(200, "text/html", confHTML);
}

void handleCSS(){
  server.send(200, "text/css", CSS);
}

void handleMainPage() {
  server.send(200, "text/html", mainHTML);
}

void handleJSMain(){
  server.send(200, "text/javascript", mainJS);
}

void handleJSForm(){
  server.send(200, "text/javascript", confJS);
}

// Restarts system
// @return 0 if success, -1 if error
int restart(){
  return -1;
}
