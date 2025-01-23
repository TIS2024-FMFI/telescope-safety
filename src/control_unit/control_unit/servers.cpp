#include "servers.h"
#include "httpHandlers.h"
#include <SimpleHOTP.h>
#include <ESP8266mDNS.h>

std::list<WiFiClient> websocketClients;
WiFiServer webSocket(81);
String GUID = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";



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


int sendToClients(AzimuthElevation* azimuthElevation){
  String message = "{\"azimuth\":";
  message += azimuthElevation->azimuth;
  message += ",";
  message += " \"elevation\"";
  message += azimuthElevation->elevation;
  message += "}";
  char header[1] = {0x81};
  String out = header;
  char len[1] = {(uint8_t)message.length()};
  out += len;
  out += message;
  Serial.println(out);
  for (WiFiClient client : websocketClients){
    client.print(out);
    client.flush();
  }
  return 1;
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
  return 1;
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
    Serial.println("Going to read Client");
    String request = client0.readString();
    String websocketKey = extractKey(request);
    if (websocketKey){
      Serial.print("Client key is: ");
      Serial.println(websocketKey);
      String acceptKey = getAcceptKey(websocketKey);
      Serial.print("Accept key is: ");
      Serial.println(acceptKey);
      client0.print(websocketAnswer(acceptKey));
      client0.flush();
    }
    // Serial.println(request);
  }
}


void websocketDisconnectInactive(){
  for (auto it = websocketClients.begin(); it != websocketClients.end();) {
    if (!it->connected()) {
      it->stop();
      it = websocketClients.erase(it);
    } else {
      ++it;
    }
  }
}

String websocketAnswer(String acceptKey){
  String out = "HTTP/1.1 101 Switching Protocols\r\n";
  out += "Upgrade: websocket\r\n";
  out += "Connection: Upgrade\r\n";
  out += "Sec-WebSocket-Accept: ";
  out += acceptKey;
  out += "\r\n\r\n";
  return out;
}

String extractKey(String request){
  int startIndex = 0;
  int newlineIndex;

  while ((newlineIndex = request.indexOf('\n', startIndex)) != -1) {
    // Extract the substring for the current line
    String line = request.substring(startIndex, newlineIndex - 1);  // -1 
    int keyIndex = line.indexOf(':');
    String key = line.substring(0, keyIndex);

    if (key.equals("Sec-WebSocket-Key")){
      return line.substring(keyIndex + 2);
    }

    startIndex = newlineIndex + 1;
  }
  if (startIndex < request.length()) {
    String line = request.substring(startIndex);
    int keyIndex = line.indexOf(':');
    String key = line.substring(0, keyIndex);

    if (key.equals("Sec-WebSocket-Key")){
      return line.substring(keyIndex + 2);
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

String getAcceptKey(String key){
  String concatenated = key + GUID;

  int ml = concatenated.length(); 
  char concatenatedArray[ml+1];
  concatenated.toCharArray(concatenatedArray, ml+1);

  ml *= 8;
  
  uint32_t hash[5] = {}; 
  SimpleSHA1::generateSHA((uint8_t*)concatenatedArray, ml, hash);
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
