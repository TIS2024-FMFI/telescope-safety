#ifndef HTTP_HANDLERS_H
#define HTTP_HANDLERS_H
#include <WebServer.h>

extern WebServer server;
extern const char *confPageFilePath;
extern const char *confJSFilePath;
extern const char *mainPageFilePath;
extern const char *mainJSFilePath;
extern const char *stylesCSSFilePath;

// Handles all links thet dont exist
void handleNotFound();

// Handles config page when data are send to server
void handleFormPOST();

// Handles config page when page is requested
void handleFormPage();

// Handles main page when page is requested
void handleMainPage();

// Handles CSS when it is requested
void handleCSS();

// Handles JS for main page when it is requested
void handleJSMain();

// Handles JS for config page when it is requested
void handleJSForm();

// Handles file downloading when it is requested
void handleFileDownload();

// Handles loading websites into RAM
void setupStaticFiles();

void restart();

// Define form names:
#define ZONE_CONFIG_BUTTON "config_zones"
#define ZONE_CONFIG_FIELD "config_zones_field"
#define ALARM_CONFIG_BUTTON "config_alarm"
#define LOG_INTERVAL_FIELD "log_interval"
#define UPDATE_INTERVAL_FIELD "update_interval"
#define ALARM_CHECKBOX "alarm_conf"
#define MOTORS_CHECKBOX "motors_conf"
#define TURN_OFF_LOGS_CHECKBOX "disable_logging"
#define RESTART_BUTTON "restart"
#define DOWNLOAD_EVENTS_BUTTON "download_events"
#define DOWNLOAD_LOGS_BUTTON "download_logs"
#define DOWNLOAD_LOGS_FROM_DATEFIELD "logs_from_date"
#define DOWNLOAD_LOGS_TO_DATEFIELD "logs_to_date"


#define MATRIX_CONFIG_BUTTON "config_matrix"

#define MATRIX00 "matrix[0][0]"
#define MATRIX01 "matrix[0][1]"
#define MATRIX02 "matrix[0][2]"
#define MATRIX03 "matrix[0][3]"
#define MATRIX10 "matrix[1][0]"
#define MATRIX11 "matrix[1][1]"
#define MATRIX12 "matrix[1][2]"
#define MATRIX13 "matrix[1][3]"
#define MATRIX20 "matrix[2][0]"
#define MATRIX21 "matrix[2][1]"
#define MATRIX22 "matrix[2][2]"
#define MATRIX23 "matrix[2][3]"
#define MATRIX30 "matrix[3][0]"
#define MATRIX31 "matrix[3][1]"
#define MATRIX32 "matrix[3][2]"
#define MATRIX33 "matrix[3][3]"

extern  const char* matrixTags[4][4];

#endif
