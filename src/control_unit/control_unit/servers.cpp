#include "servers.h"
#include "httpHandlers.h"
#include <ESP8266mDNS.h>



int setupHTTPServer(){
  server.on("/", handleMainPage);
  server.on("/config", HTTP_GET, handleFormPage);
  server.on("/config", HTTP_POST, handleFormPOST);
  server.on("/styles.css", handleCSS);
  server.on("/form.js", handleJSForm);
  server.on("/main.js", handleJSMain);
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP server started");
  return 1;
}


int setupWebSocketServer(){
  return -1;
}


int setupMDNSServer(){
  Serial.println("Starting mDNS!");
  if (MDNS.begin("telescop")) {
    Serial.println("MDNS responder started");
  }
  return 1;
}


int setupServers(){
  if (!setupHTTPServer()){
    return -1;
  }
  if (!setupWebSocketServer()){
    return -2;
  }
  if (!setupMDNSServer()){
    return -3;
  }
  return 1;
}