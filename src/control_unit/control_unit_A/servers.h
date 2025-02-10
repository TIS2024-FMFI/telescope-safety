#ifndef SERVERS_H
#define SERVERS_H

#include "common_structs.h"
#include <list>
#include <WiFiClient.h>
#include <WebServer.h>

extern std::list<WiFiClient> websocketClients;
extern WiFiServer webSocket;
extern WebServer server;


// Setup HTTP server
// @return 0 if success, -1 if error
int setupHTTPServer();

// Setup WebSocket server
// @return 0 if success, -1 if error
int setupWebSocketServer();

// Setup mDNS server
// @return 0 if success, -1 if error
int setupMDNSServer();

// Starts all servers
// @return 0 if success, -1 if error
int setupServers();

// Sends AzimuthElevation structure to all conected clients via WS
// @param azimutElevation pointer to AzimuthElevation structure
// @param error error message
// @param warning warning message
// @return 0 if success, -1 if error
int sendToClients(AzimuthElevation* azimuthElevation, String error = "", String warning = "");

// Finds the WS key from request
// @param client that sends the request
// @return WS key
String extractKey(WiFiClient& client);

// Generates the Accept key from the WS request key
// @param key WS request key
// @return WS accept key
String getAcceptKey(String& key);

// Converts HASH value into bytes
// @param hash array with the HASH
// @param hashBytes array that will be filled with converted hash
void convertHashToBytes(const uint32_t hash[5], uint8_t hashBytes[20]);

// Generates the WS answer on WS request
// @param acceptKey WS accept key
// @return WS accept answer
String websocketAnswer(String& acceptKey);

// Encode data to base64
// @param data pointer to the data that should be encoded
// @param len lenght of the data
// @return Encoded String
String base64Encode(const uint8_t *data, size_t len);

// Shold be called every loop for ES to work
void websocketLoop();

// Chceks the connections and cloases inactive ones
void websocketDisconnectInactive();

// Handles all incomming WS requests
void websocketConnectIncomming();

#endif
