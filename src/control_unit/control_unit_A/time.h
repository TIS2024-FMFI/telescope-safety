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
// @return Time struct
Time getRealTime();

// Sets the year month and day into given Time struct
// @param time struct into which current year month and day should be loaded
// @return 0 if success, -1 if error
int getYearMonthDay(Time *time);

// Checks if the year is leep or not
// @param year which should be checked
// @return true if year is leep, false if its not
bool isLeapYear(int year);

// Converts Time struct into string format
// @param time that should be turned into string
// @return date in String format
String dateToString(Time time);

// Converts Time struct into string format
// @param time that should be turned into string
// @return date in String format
String timeToString(Time time);
#endif