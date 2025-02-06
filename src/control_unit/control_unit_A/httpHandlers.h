#ifndef HTTP_HANDLERS_H
#define HTTP_HANDLERS_H
#include <WebServer.h>

extern WebServer server;
extern const char *confPageFilePath;
extern const char *confJSFilePath;
extern const char *mainPageFilePath;
extern const char *mainJSFilePath;
extern const char *stylesCSSFilePath;

void handleNotFound();
void handleFormPOST();
void handleFormPage();
void handleMainPage();
void handleCSS();
void handleJSMain();
void handleJSForm();
void handleFileDownload();
void setupStaticFiles();


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
#define MATRIX10 "matrix[1][0]"
#define MATRIX11 "matrix[1][1]"
#define MATRIX12 "matrix[1][2]"
#define MATRIX20 "matrix[2][0]"
#define MATRIX21 "matrix[2][1]"
#define MATRIX22 "matrix[2][2]"

extern  const char* matrixTags[3][3];

#endif
