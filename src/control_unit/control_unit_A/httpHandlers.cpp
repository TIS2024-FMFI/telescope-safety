#include "httpHandlers.h"
#include <Arduino.h>
#include "logs.h"
#include <SD.h>
#include "forbidden_zones_config_parse.h"
#include "lora_communication.h"

#define DEBUG 1
#define PRELOAD 0

const char *confPageFilePath = "/www/config.html";
const char *confJSFilePath = "/www/config.js";
const char *mainPageFilePath = "/www/index.html";
const char *mainJSFilePath = "/www/main.js";
const char *stylesCSSFilePath = "/www/styles.css";


#if PRELOAD

String mainHTML;
String mainJS;
String stylesCSS;
String confJS;
String confHTML1;
String confHTML2;
String confHTML3;
String confHTML4;
String confHTML5;
String confHTML6;
String confHTML7;

#endif


int restart();


#if PRELOAD
int loadConf(const char* filePath){
  File file = SD.open(filePath, FILE_READ);
  if (!file) {
    Serial.println("umrel som");
    return -1; 
  }
  
  Serial.println("ide sa na to!");
  //load until zone_field
  confHTML1.reserve(100);
  while (1){
    int ch = file.read();
    if (ch == -1){
      break;
    }
    char c = ch;
    confHTML1.concat(c);
    if (confHTML1.endsWith(ZONE_CONFIG_FIELD)){
      break;
    }
  }
  confHTML1.concat(file.readStringUntil('>'));  
  confHTML1.concat(">");

  // load intil alarm
  confHTML2.reserve(100);
  while (1){
    int ch = file.read();
    if (ch == -1){
      break;
    }
    char c = ch;
    confHTML2.concat(c);
    if (confHTML2.endsWith(ALARM_CHECKBOX)){
      break;
    }
  }
  confHTML2.concat(file.readStringUntil(' '));
  confHTML2.concat(" ");

  // load intil RELE
  confHTML3.reserve(100);
  while (1){
    int ch = file.read();
    if (ch == -1){
      break;
    }
    char c = ch;
    confHTML3.concat(c);
    if (confHTML3.endsWith(MOTORS_CHECKBOX)){
      break;
    }
  }
  confHTML3.concat(file.readStringUntil(' '));
  confHTML3.concat(" ");


  confHTML4.reserve(100);
  while (1){
    int ch = file.read();
    if (ch == -1){
      break;
    }
    char c = ch;
    confHTML4.concat(c);
    if (confHTML4.endsWith(UPDATE_INTERVAL_FIELD)){
      break;
    }
  }
  confHTML4.concat(file.readStringUntil(' '));
  confHTML4.concat(" ");

  confHTML5.reserve(100);
  while (1){
    int ch = file.read();
    if (ch == -1){
      break;
    }
    char c = ch;
    confHTML5.concat(c);
    if (confHTML5.endsWith(LOG_INTERVAL_FIELD)){
      break;
    }
  }
  confHTML5.concat(file.readStringUntil(' '));
  confHTML5.concat(" ");

  confHTML6.reserve(100);
  while (1){
    int ch = file.read();
    if (ch == -1){
      break;
    }
    char c = ch;
    confHTML6.concat(c);
    if (confHTML6.endsWith(TURN_OFF_LOGS_CHECKBOX)){
      break;
    }
  }
  confHTML6.concat(file.readStringUntil(' '));
  confHTML6.concat(" ");

  confHTML7 = file.readString();

  return 0;
}

#else

String loadConf(const char* filePath){
  String response;
  response.reserve(3950);
  File file = SD.open(filePath, FILE_READ);
  if (!file) {
    Serial.println("umrel som");
    return ""; 
  }
  while (1){
    int ch = file.read();
    if (ch == -1){
      break;
    }
    char c = ch;
    response.concat(c);
    if (response.endsWith(ZONE_CONFIG_FIELD)){
      break;
    }
  }
  response.concat(file.readStringUntil('>'));  
  response.concat(">");
  char* zones = loadFile(forbiddenConfigFilePath);
  response.concat(zones);
  free(zones);

  while (1){
    int ch = file.read();
    if (ch == -1){
      break;
    }
    char c = ch;
    response.concat(c);
    if (response.endsWith(ALARM_CHECKBOX)){
      break;
    }
  }
  response.concat(file.readStringUntil(' '));
  response.concat(" ");

  if (settings.alarm){
    response.concat("checked ");
  }

  while (1){
    int ch = file.read();
    if (ch == -1){
      break;
    }
    char c = ch;
    response.concat(c);
    if (response.endsWith(MOTORS_CHECKBOX)){
      break;
    }
  }
  response.concat(file.readStringUntil(' '));
  response.concat(" ");

  if (settings.rele){
    response.concat("checked ");
  }

  while (1){
    int ch = file.read();
    if (ch == -1){
      break;
    }
    char c = ch;
    response.concat(c);
    if (response.endsWith(UPDATE_INTERVAL_FIELD)){
      break;
    }
  }
  response.concat(file.readStringUntil(' '));
  response.concat(" ");

  response.concat("value=\"");
  response.concat(settings.update_frequency);
  response.concat("\" ");  

  while (1){
    int ch = file.read();
    if (ch == -1){
      break;
    }
    char c = ch;
    response.concat(c);
    if (response.endsWith(LOG_INTERVAL_FIELD)){
      break;
    }
  }
  response.concat(file.readStringUntil(' '));
  response.concat(" ");

  response.concat("value=\"");
  response.concat(settings.log_frequency);
  response.concat("\" ");

  while (1){
    int ch = file.read();
    if (ch == -1){
      break;
    }
    char c = ch;
    response.concat(c);
    if (response.endsWith(TURN_OFF_LOGS_CHECKBOX)){
      break;
    }
  }
  response.concat(file.readStringUntil(' '));
  response.concat(" ");

  if (settings.logging){
    response.concat("checked ");
  }

  response.concat(file.readString());

  return response;
}

#endif

void setupStaticFiles(){
  #if PRELOAD
  mainHTML = loadFile(mainPageFilePath);
  stylesCSS = loadFile(stylesCSSFilePath);
  mainJS = loadFile(mainJSFilePath);
  confJS = loadFile(confJSFilePath);
  loadConf(confPageFilePath);
  #endif
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
  Serial.println("I got POST");
  #if DEBUG
  String message = "";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  Serial.println(message);
  #endif

  const char *clientIP = server.client().remoteIP().toString().c_str();
  //Serial.print("Check: ");
  //Serial.println(server.arg(ZONE_CONFIG_BUTTON));
  if (server.arg(ZONE_CONFIG_BUTTON) != ""){
    Serial.println("POST zones");
    String xx = server.arg(ZONE_CONFIG_FIELD);
    xx.replace("\r\n","\n");
    const char* newZones = xx.c_str();
    if (setUpZones(newZones) == 0 && writeNewForbiddenConfig(newZones) == 0){
      Serial.println("Written zones");
      writeChangeToLog(FORBIDDEN_ZONE_CHANGED, clientIP);
    }
  }
  else if (server.arg(ALARM_CONFIG_BUTTON) != ""){
    Serial.println("POST alarm");
    settings.update_frequency = server.arg(UPDATE_INTERVAL_FIELD).toInt();
    settings.log_frequency = server.arg(LOG_INTERVAL_FIELD).toInt();
    settings.alarm = bool(server.arg(ALARM_CHECKBOX).toInt());
    settings.rele = bool(server.arg(MOTORS_CHECKBOX).toInt());
    settings.logging = bool(server.arg(TURN_OFF_LOGS_CHECKBOX).toInt());
    if (writeConfigAlarmAndIntervals(settings) == 0){
      Serial.println("Written alarm");
      writeChangeToLog(LOG_FREQUENCY_AND_ALARM_TYPE_CHANGED, clientIP);
    }

  }
  else if (server.arg(RESTART_BUTTON) != ""){
    Serial.println("POST restart");
    if (restart() == 0){
      Serial.println("restarting...");
      writeChangeToLog(RESTART, clientIP);
    }
  }
  handleFormPage();
}



void handleFormPage(){
  #if PRELOAD
  String response = confHTML1;
  // If this takes to much time we should keep somewhere saved the zones String config
  response.concat(loadFile(forbiddenConfigFilePath));
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
  #else
  String response = loadConf(confPageFilePath);
  #endif
  server.send(200, "text/html", response);
}

void handleCSS(){
  #if PRELOAD
  server.send(200, "text/css", stylesCSS);
  #else
  char* response = loadFile(stylesCSSFilePath);
  server.send(200, "text/css", response);
  free(response);
  #endif
}

void handleMainPage() {
  #if PRELOAD
  server.send(200, "text/html", mainHTML);
  #else
  char* response = loadFile(mainPageFilePath);
  server.send(200, "text/html", response);
  free(response);
  #endif
}

void handleJSMain(){
  #if PRELOAD
  server.send(200, "text/javascript", mainJS);
  #else
  char* response = loadFile(mainJSFilePath);
  server.send(200, "text/javascript", response);
  free(response);
  #endif
}

void handleJSForm(){
  #if PRELOAD
  server.send(200, "text/javascript", confJS);
  #else
  char* response = loadFile(confJSFilePath);
  server.send(200, "text/javascript", response);
  free(response);
  #endif
  
}

// Restarts system
// @return 0 if success, -1 if error
int restart(){
  return restartInertialUnit(-1);
}