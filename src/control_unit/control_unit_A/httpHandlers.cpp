#include "httpHandlers.h"
#include <Arduino.h>
#include "logs.h"
#include <SD.h>
#include "forbidden_zones_config_parse.h"
#include "lora_communication.h"

#define DEBUG 1
String mainHTML;
String mainJS;
String CSS;
String confJS;
String confHTML1;
String confHTML2;
String confHTML3;
String confHTML4;
String confHTML5;
String confHTML6;
String confHTML7;


int restart();
int loadConf(const char* filePath);


void setupStaticFiles(){
  loadConf("/www/config.html");
  mainHTML = loadFile("/www/index.html");
  CSS = loadFile("/www/style.html");
  mainJS = loadFile("/www/main.js");
  confJS = loadFile("/www/config.js");
}

int loadConf(const char* filePath){
  File file = SD.open(filePath, FILE_READ);
  if (!file) {
    return -1; 
  }
  

  //load until zone_field
  confHTML1.reserve(100);
  while (1){
    confHTML1.concat(file.read());
    if (confHTML1.endsWith(ZONE_CONFIG_FIELD)){
      break;
    }
  }
  confHTML1.concat(file.readStringUntil('>'));  
  confHTML1.concat(">");

  // load intil alarm
  confHTML2.reserve(100);
  while (1){
    confHTML2.concat(file.read());
    if (confHTML2.endsWith(ALARM_CHECKBOX)){
      break;
    }
  }
  confHTML2.concat(file.readStringUntil(' '));
  confHTML1.concat(" ");

  // load intil RELE
  confHTML3.reserve(100);
  while (1){
    confHTML3.concat(file.read());
    if (confHTML3.endsWith(MOTORS_CHECKBOX)){
      break;
    }
  }
  confHTML3.concat(file.readStringUntil(' '));
  confHTML1.concat(" ");


  confHTML4.reserve(100);
  while (1){
    confHTML4.concat(file.read());
    if (confHTML4.endsWith(UPDATE_INTERVAL_FIELD)){
      break;
    }
  }
  confHTML4.concat(file.readStringUntil(' '));
  confHTML1.concat(" ");

  confHTML5.reserve(100);
  while (1){
    confHTML5.concat(file.read());
    if (confHTML5.endsWith(LOG_INTERVAL_FIELD)){
      break;
    }
  }
  confHTML5.concat(file.readStringUntil(' '));
  confHTML1.concat(" ");

  confHTML6.reserve(100);
  while (1){
    confHTML6.concat(file.read());
    if (confHTML6.endsWith(TURN_OFF_LOGS_CHECKBOX)){
      break;
    }
  }
  confHTML6.concat(file.readStringUntil(' '));
 confHTML1.concat(" ");

  confHTML7 = file.streamRemaining();
  return 0;
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
    settings.update_frequency = server.arg(UPDATE_INTERVAL_FIELD).toInt();
    settings.log_frequency = server.arg(LOG_INTERVAL_FIELD).toInt();
    settings.alarm = bool(server.arg(ALARM_CHECKBOX).toInt());
    settings.rele = bool(server.arg(MOTORS_CHECKBOX).toInt());
    settings.logging = bool(server.arg(TURN_OFF_LOGS_CHECKBOX).toInt());
    if (writeConfigAlarmAndIntervals(settings)){
      writeChangeToLog(LOG_FREQUENCY_AND_ALARM_TYPE_CHANGED, clientIP);
    }

  }
  else if (server.arg(RESTART_BUTTON)){
    if (restart()){
      writeChangeToLog(RESTART, clientIP);
    }
  }
  handleFormPage();
}



void handleFormPage(){
  String response = confHTML1;
  // If this takes to much time we should keep somewhere saved the zones String config
  response.concat(loadFile("/conf/zones.txt"));
  response.concat(confHTML2);
  if (settings.alarm){
    response.concat("checked ");
  }
  response.concat(confHTML3);
  if (settings.rele){
    response.concat("checked ");
  }
  response.concat(confHTML4);
  response.concat("value=\"");
  response.concat(settings.update_frequency);
  response.concat("\" ");
  response.concat(confHTML5);
  response.concat("value=\"");
  response.concat(settings.log_frequency);
  response.concat("\" ");
  response.concat(confHTML6);
  if (settings.logging){
    response.concat("checked ");
  }
  response.concat(confHTML7);
  server.send(200, "text/html", response);
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
  return restartInertialUnit(-1);
}
