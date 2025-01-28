#ifndef TIME_H
#define TIME_H

#include <Arduino.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

extern NTPClient timeClient;

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
int getYearMonthDay(Time *time);
bool isLeapYear(int year);
String dateToString(Time time);
String timeToString(Time time);
#endif