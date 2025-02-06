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

 const char* matrixTags[3][3] = {{MATRIX00, MATRIX01, MATRIX02},
                    {MATRIX10, MATRIX11, MATRIX12},
                    {MATRIX20, MATRIX21, MATRIX22}};


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
String confHTML8;
String confHTML9;
String confHTML10;
String confHTML11;
String confHTML12;
String confHTML13;
String confHTML14;
String confHTML15;
String confHTML16;

String confTags[3][3] = {{confHTML2, confHTML3, confHTML4},
                         {confHTML5, confHTML6, confHTML7},
                         {confHTML8, confHTML9, confHTML10}};

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

  for (int i = 0; i < 3; i++){
    for (int ii = 0; ii < 3; ii++){
      while (1){
        int ch = file.read();
        if (ch == -1){
          break;
        }
        char c = ch;
        confTags[i][ii].concat(c);
        if (confTags[i][ii].endsWith(matrixTags[i][ii])){
          break;
        }
      }
      confTags[i][ii].concat(file.readStringUntil(' '));  
      confTags[i][ii].concat(" ");
    }
  }

  // load intil alarm
  confHTML11.reserve(100);
  while (1){
    int ch = file.read();
    if (ch == -1){
      break;
    }
    char c = ch;
    confHTML11.concat(c);
    if (confHTML11.endsWith(ALARM_CHECKBOX)){
      break;
    }
  }
  confHTML11.concat(file.readStringUntil(' '));
  confHTML11.concat(" ");

  // load intil RELE
  confHTML12.reserve(100);
  while (1){
    int ch = file.read();
    if (ch == -1){
      break;
    }
    char c = ch;
    confHTML12.concat(c);
    if (confHTML12.endsWith(MOTORS_CHECKBOX)){
      break;
    }
  }
  confHTML12.concat(file.readStringUntil(' '));
  confHTML12.concat(" ");


  confHTML13.reserve(100);
  while (1){
    int ch = file.read();
    if (ch == -1){
      break;
    }
    char c = ch;
    confHTML13.concat(c);
    if (confHTML13.endsWith(UPDATE_INTERVAL_FIELD)){
      break;
    }
  }
  confHTML13.concat(file.readStringUntil(' '));
  confHTML13.concat(" ");

  confHTML14.reserve(100);
  while (1){
    int ch = file.read();
    if (ch == -1){
      break;
    }
    char c = ch;
    confHTML14.concat(c);
    if (confHTML14.endsWith(LOG_INTERVAL_FIELD)){
      break;
    }
  }
  confHTML14.concat(file.readStringUntil(' '));
  confHTML14.concat(" ");

  confHTML15.reserve(100);
  while (1){
    int ch = file.read();
    if (ch == -1){
      break;
    }
    char c = ch;
    confHTML15.concat(c);
    if (confHTML15.endsWith(TURN_OFF_LOGS_CHECKBOX)){
      break;
    }
  }
  confHTML15.concat(file.readStringUntil(' '));
  confHTML15.concat(" ");

  confHTML16 = file.readString();

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

  for (int i = 0; i < 3; i++){
    for (int ii = 0; ii < 3; ii++){
      while (1){
        int ch = file.read();
        if (ch == -1){
          break;
        }
        char c = ch;
        response.concat(c);
        if (response.endsWith(matrixTags[i][ii])){
          break;
        }
      }
      response.concat(file.readStringUntil(' '));  
      response.concat(" ");
      response.concat("value=\"");
      response.concat(TransformMatrix[i][ii]);
      response.concat("\" ");

    }
  }


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

  String clientIPStr = server.client().remoteIP().toString();
  const char *clientIP = clientIPStr.c_str();
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
  else if (server.arg(MATRIX_CONFIG_BUTTON) != "") {
    for (int i = 0; i < 3; i++){
      for (int ii = 0; ii < 3; ii++){
        TransformMatrix[i][ii] = server.arg(matrixTags[i][ii]).toInt();
      }
    }
    if (saveMatrix() == 0) {
      writeChangeToLog(TRANSFORM_MATRIX_CHANGED, clientIP);
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

  for (int i = 0; i < 3; i++){
    for (int ii = 0; ii < 3; ii++){
      response.concat(confTags[i][ii]);
      response.concat("value=\"");
      response.concat(matrixTags[i][ii]);
      response.concat("\" ");
    }
  }

  response.concat(confHTML11);
  if (settings.alarm){
    response.concat("checked ");
  }
  response.concat(confHTML12);
  if (settings.rele){
    response.concat("checked ");
  }
  response.concat(confHTML13);
  response.concat("value=\"");
  response.concat(settings.update_frequency);
  response.concat("\" ");
  response.concat(confHTML14);
  response.concat("value=\"");
  response.concat(settings.log_frequency);
  response.concat("\" ");
  response.concat(confHTML15);
  if (settings.logging){
    response.concat("checked ");
  }
  response.concat(confHTML16);
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
