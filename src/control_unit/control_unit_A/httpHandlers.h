#include <WebServer.h>

extern WebServer server;

void handleNotFound();
void handleFormPOST();
void handleFormPage();
void handleMainPage();
void handleCSS();
void handleJSMain();
void handleJSForm();
void handleFileDownload();


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
