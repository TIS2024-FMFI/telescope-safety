#include "servers.h"
#include "httpHandlers.h"
#include <SimpleHOTP.h>
#include <ESP8266mDNS.h>

#include <W55RP20lwIP.h> // Include library for the right board
extern Wiznet55rp20lwIP eth;

std::list<WiFiClient> websocketClients;
WiFiServer webSocket(81);
String GUID = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
WebServer server(80);



int setupHTTPServer(){
  server.on("/", handleMainPage);
  server.on("/config", HTTP_GET, handleFormPage);
  server.on("/config", HTTP_POST, handleFormPOST);
  server.on("/styles.css", handleCSS);
  server.on("/config.js", handleJSForm);
  server.on("/main.js", handleJSMain);
  server.on("/download", handleFileDownload);
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP server started");
  return 0;
}

int lastUpdateClients = 0;
const int secendsToMilis = 1000;


int sendToClients(AzimuthElevation* azimutElevation, String error, String warning) {
  if (eth.status() == WL_CONNECTED &&   //probably will be connected, when clients are connected but to be sure
      websocketClients.size() &&
      millis() - lastUpdateClients >= (settings.update_frequency * secendsToMilis)){
    lastUpdateClients = millis();
    String message;
    message.reserve(45);
    message.concat("{\"azimuth\":");
    message.concat(String(azimuthElevation->azimuth, 2));
    message.concat(",\"elevation\":");
    message.concat(String(azimuthElevation->elevation, 2));
    if (error != ""){
      message.concat(",\"error\":");
      message.concat(error);
    }
    if (warning != ""){
      message.concat(",\"warning\":");
      message.concat(warning);
    }
    message.concat("}");

    size_t payloadLength = message.length();

    std::vector<uint8_t> frame;
    frame.push_back(0x81);

    if (payloadLength <= 125) {
      frame.push_back(payloadLength);
    } else if (payloadLength <= 65535) {
      frame.push_back(126);
      frame.push_back((payloadLength >> 8) & 0xFF);
      frame.push_back(payloadLength & 0xFF);
    } else {
      frame.push_back(127);
      for (int i = 7; i >= 0; --i) {
        frame.push_back((payloadLength >> (i * 8)) & 0xFF);
      }
    }
    frame.insert(frame.end(), message.begin(), message.end());

    for (WiFiClient client : websocketClients) {
      if (client.connected()) {
        client.write(frame.data(), frame.size());
      }
    }
  }
  return 0;
}


int setupWebSocketServer(){
  webSocket.begin();
  return 0;
}


int setupMDNSServer(){
  Serial.println("Starting mDNS!");
  if (MDNS.begin("telescop")) {
    Serial.println("MDNS responder started");
  }
  return 0;
}


int setupServers(){
  if (setupHTTPServer() != 0){
    return -1;
  }
  if (setupWebSocketServer() != 0){
    return -2;
  }
  if (setupMDNSServer() != 0){
    return -3;
  }
  return 0;
}

void websocketLoop(){
  websocketConnectIncomming();
  websocketDisconnectInactive();
}

void websocketConnectIncomming(){
  WiFiClient client0 = webSocket.accept();
  if(client0.available()) {
    websocketClients.push_back(client0);
    String websocketKey = extractKey(client0);
    if (websocketKey){
      String acceptKey = getAcceptKey(websocketKey);
      client0.print(websocketAnswer(acceptKey));
    }
  }
}

void websocketDisconnectInactive(){
  for (auto client = websocketClients.begin(); client != websocketClients.end();) {
    if (!client->connected()) {
      client->stop();
      client = websocketClients.erase(client);
    } else {
      ++client;
    }
  }
}

String websocketAnswer(String& acceptKey) {
    return "HTTP/1.1 101 Switching Protocols\r\n"
           "Upgrade: websocket\r\n"
           "Connection: Upgrade\r\n"
           "Sec-WebSocket-Accept: " + acceptKey + "\r\n\r\n";
}


String extractKey(WiFiClient& client){
  String request;
  request.reserve(100);
  const int timeout = 500; // 500ms timeout
  unsigned long startMillis = millis();
  bool keyExtraction = false;

  while (client.connected() && (millis() - startMillis < timeout)) {
    while (client.available()) {
      char c = client.read();
      request.concat(c);
      if (keyExtraction && request.endsWith("\r\n")){
        return request.substring(request.lastIndexOf(" ") + 1, request.lastIndexOf("\r"));
      }
      else if (request.endsWith("Sec-WebSocket-Key: ")){
        keyExtraction = true;
      }
    }
  }
  return "";
}

void convertHashToBytes(const uint32_t hash[5], uint8_t hashBytes[20]) {
    for (int i = 0; i < 5; ++i) {
        hashBytes[i * 4 + 0] = (hash[i] >> 24) & 0xFF; // Most significant byte
        hashBytes[i * 4 + 1] = (hash[i] >> 16) & 0xFF;
        hashBytes[i * 4 + 2] = (hash[i] >> 8) & 0xFF;
        hashBytes[i * 4 + 3] = (hash[i]) & 0xFF;      // Least significant byte
    }
}

String getAcceptKey(String& key){
  String concatenated = key + GUID;

  int len = concatenated.length(); 
  char concatenatedArray[len+1];
  concatenated.toCharArray(concatenatedArray, len+1);

  len *= 8;
  
  uint32_t hash[5] = {}; 
  SimpleSHA1::generateSHA((uint8_t*)concatenatedArray, len, hash);
  uint8_t hashed[20];
  convertHashToBytes(hash, hashed);

  String acceptKey = base64Encode(hashed, 20);
  return acceptKey;
}


String base64Encode(const uint8_t *data, size_t len) {
    const char base64Chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    String result = "";
    uint8_t buf[4];
    int i;

    for (i = 0; i < len; i += 3) {
        buf[0] = (data[i] >> 2) & 0x3F;
        buf[1] = (data[i] & 0x03) << 4;
        if (i + 1 < len) {
            buf[1] |= (data[i + 1] >> 4) & 0x0F;
            buf[2] = (data[i + 1] & 0x0F) << 2;
            if (i + 2 < len) {
                buf[2] |= (data[i + 2] >> 6) & 0x03;
                buf[3] = data[i + 2] & 0x3F;
            } else {
                buf[3] = 64;  // Padding
            }
        } else {
            buf[2] = buf[3] = 64;  // Padding
        }

        for (int j = 0; j < 4; j++) {
            result += (buf[j] < 64) ? base64Chars[buf[j]] : '=';
        }
    }

    return result;
}
