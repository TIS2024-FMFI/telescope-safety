#include <time.h>
#include <Arduino.h>
#include "allINeedForControlUnit.h"


int writeNewForbiddenConfig(String newConfiguration){
  return -1;
}

int writeNewLogFrequency(int logFrequency, int clientUpdateFrequency){
  return -1;
}

int writeNewAlarmType(bool alarm, bool motors){
  return -1;
}

int writeChangeToLog(ChangeType changeType){
  return -1;
}

int writeAlarmToLog(AzimuthElevation* azimutElevation){
  return -1;
}

int restart(){
  return -1;
}

int downloadEventLogs(){
  return -1;
}

int downloadLogs(String from, String to){
  return -1;
}




