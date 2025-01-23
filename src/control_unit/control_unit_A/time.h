#include <Arduino.h>

struct Time{
  int year;
  int month;
  int day;
  int hours;
  int minutes;
  int seconds;
};

// Retrievs real time from Ethernet
// @return time in seconds
Time getRealTime();
int getYear();
int getMonth();
int getDay();
String timeToString(Time time);
