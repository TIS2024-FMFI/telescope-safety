
#include <W55RP20lwIP.h> // Include library for the right board
Wiznet55rp20lwIP eth(1 /* chip select */);

#include <WiFiClient.h>
#include <WebServer.h> 
#include <LEAmDNS.h>
#include <StreamString.h>
#include "httpHandlers.h"
#include "servers.h"
#include <SimpleHOTP.h>


WiFiServer webSocket(81);
std::list<WiFiClient> websocketClients;
WebServer server(80);
String GUID = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";


// Functions
void setupEthernet();
String extractKey(String request);
String getAcceptKey(String key);
void convertHashToBytes(const uint32_t hash[5], uint8_t hashBytes[20]);
String websocketAnswer(String acceptKey);
String base64Encode(const uint8_t *data, size_t len);

void setup(void) {
  Serial.begin(115200);
  delay(5000);
  Serial.println("Started Serial");

  setupEthernet();
  setupHTTPServer();

  webSocket.begin();


  Serial.println("Starting mDNS!");
  if (MDNS.begin("picow")) {
    Serial.println("MDNS responder started");
  }

}
int i = 0;
void loop(void) {
  server.handleClient();
  MDNS.update();

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
  for (auto it = websocketClients.begin(); it != websocketClients.end();) {
    if (!it->connected()) {
      it->stop();
      it = websocketClients.erase(it);
    } else {
      ++it;
    }
  }

  if (i % 5000){
    AzimuthElevation azel;
    azel.azimuth = i % 1500;
    azel.elevation = i % 2500;
    sendToClients(&azel);
  }
  i++;
  // Serial.println(websocketClients.size());

}





// Functions

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



void setupEthernet(){
  Serial.println("Starting ETH");
  // Start the Ethernet port
  if (!eth.begin()) {
    Serial.println("No wired Ethernet hardware detected. Check pinouts, wiring.");
    while (1) {
      delay(1000);
    }
  }

  // Wait for connection
  while (!eth.connected()) {
    Serial.print("_");
    delay(500);
  }
  while (eth.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.print("IP address: ");
  Serial.println(eth.localIP());
}

