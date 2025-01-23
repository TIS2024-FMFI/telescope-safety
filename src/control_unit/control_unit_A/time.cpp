#include"time.h"
#include <NTPClient.h>

extern NTPClient timeClient;



Time getRealTime(){
  Time out;
  out.year = getYear();
  out.month = getMonth();
  out.day = getDay();
  out.hours = timeClient.getHours();
  out.minutes = timeClient.getMinutes();
  out.seconds = timeClient.getSeconds();

  return out;
}


String timeToString(Time time) {
  String formattedTime = String(time.year) + "-";
  
  // Add leading zero for month, day, hours, minutes, and seconds if necessary
  formattedTime += (time.month < 10 ? "0" : "") + String(time.month) + "-";
  formattedTime += (time.day < 10 ? "0" : "") + String(time.day) + " ";
  formattedTime += (time.hours < 10 ? "0" : "") + String(time.hours) + ":";
  formattedTime += (time.minutes < 10 ? "0" : "") + String(time.minutes) + ":";
  formattedTime += (time.seconds < 10 ? "0" : "") + String(time.seconds);
  
  return formattedTime;
}



int getYear() {
    unsigned long epochTime = timeClient.getEpochTime();
    
    // Number of seconds in a non-leap year
    const unsigned long seconds_in_year = 31536000;
    
    // Calculate number of years since 1970
    int years = epochTime / seconds_in_year;
    
    // Adjust for leap years: Every 4 years is a leap year, except centuries not divisible by 400
    unsigned long remaining_seconds = epochTime % seconds_in_year;
    int days = remaining_seconds / 86400;  // Number of days in the current year

    // Determine leap year (simple rule: years divisible by 4 are leap years, except century years not divisible by 400)
    if ((years % 4 == 0 && years % 100 != 0) || (years % 400 == 0)) {
        // Leap year, so adjust the days count for February 29th
        if (days > 59) {  // After Feb 29th in leap year
            days--;
        }
    }

    // Now that we have the number of days, calculate the correct year
    if (days < 0) {
        years--;  // If it's earlier than Jan 1st of the current year, subtract one from the year
    }

    return 1970 + years;  // Return the correct year
}

int getMonth() {
    unsigned long epochTime = timeClient.getEpochTime();

    // Days in each month for a non-leap year and a leap year
    const int days_in_months_normal_year[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    const int days_in_months_leap_year[] = {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    // Start from 1970
    int year = 1970;

    // Calculate the total number of days since 1970
    unsigned long days = epochTime / 86400;
    unsigned long seconds_remaining = epochTime % 86400;

    // Determine the current year by subtracting days year by year
    while (true) {
        bool isLeapYear = ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0));
        int days_in_year = isLeapYear ? 366 : 365;

        if (days < days_in_year) {
            break;
        }

        days -= days_in_year;
        year++;
    }

    // Determine the month
    const int* months_array = ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0))
                                  ? days_in_months_leap_year
                                  : days_in_months_normal_year;

    int month = 0;
    while (days >= months_array[month]) {
        days -= months_array[month];
        month++;
    }

    // Return the month (1-based: January = 1, December = 12)
    return month + 1;
}

int getDay() {
    unsigned long epochTime = timeClient.getEpochTime();

    // Days in each month for a non-leap year and a leap year
    const int days_in_months_normal_year[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    const int days_in_months_leap_year[] = {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    // Start from 1970
    int year = 1970;

    // Calculate the total number of days since 1970
    unsigned long days = epochTime / 86400;
    unsigned long seconds_remaining = epochTime % 86400;

    // Determine the current year by subtracting days year by year
    while (true) {
        bool isLeapYear = ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0));
        int days_in_year = isLeapYear ? 366 : 365;

        if (days < days_in_year) {
            break;
        }

        days -= days_in_year;
        year++;
    }

    // Determine the month
    const int* months_array = ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0))
                                  ? days_in_months_leap_year
                                  : days_in_months_normal_year;

    int month = 0;
    while (days >= months_array[month]) {
        days -= months_array[month];
        month++;
    }

    // Remaining days represent the current day of the month
    return days + 1;  // Day is 1-based
}