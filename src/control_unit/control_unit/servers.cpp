#include "servers.h"
#include "httpHandlers.h"



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
  return -1;
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