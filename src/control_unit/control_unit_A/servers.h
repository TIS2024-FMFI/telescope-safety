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
// @return 0 if success, -1 if error
int sendToClients(AzimuthElevation* azimutElevation);

String extractKey(String request);
String getAcceptKey(String key);
void convertHashToBytes(const uint32_t hash[5], uint8_t hashBytes[20]);
String websocketAnswer(String acceptKey);
String base64Encode(const uint8_t *data, size_t len);
void websocketLoop();
void websocketDisconnectInactive();
void websocketConnectIncomming();

