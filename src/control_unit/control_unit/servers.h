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