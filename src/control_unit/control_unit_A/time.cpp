#include"time.h"



WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "sk.pool.ntp.org", 3600);



Time getRealTime(){
  Time time;
  getYearMonthDay(&time);
  time.hours = timeClient.getHours();
  time.minutes = timeClient.getMinutes();
  time.seconds = timeClient.getSeconds();

  return time;
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

bool isLeapYear(int year){
  bool year_divisible_by_4 = year % 4 == 0;
  bool year_divisible_by_100 = year % 100 == 0;
  bool year_divisible_by_400 = year % 400 == 0;
  // Determine leap year (simple rule: years divisible by 4 are leap years, except century years not divisible by 400)
  return (year_divisible_by_4 && !year_divisible_by_100) || year_divisible_by_400;
}

int getYearMonthDay(Time *time) {
    unsigned long epochTime = timeClient.getEpochTime();

    // Days in each month for a non-leap year and a leap year
    const int days_in_months_normal_year[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    const int days_in_months_leap_year[] = {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    // Start from 1970
    int year = 1970;

    // Calculate the total number of days since 1970
    const int seconds_in_day = 86400;
    unsigned long days = epochTime / seconds_in_day;
    // unsigned long seconds_remaining = epochTime % seconds_in_day;

    // Determine the current year by subtracting days year by year
    while (true) {
        int days_in_leapYear = 366;
        int days_in_nonLeapYear = 365;
        int days_in_year = isLeapYear(year) ? days_in_leapYear : days_in_nonLeapYear;

        if (days < days_in_year) {
            break;
        }

        days -= days_in_year;
        year++;
    }

    // Determine the month
    const int* months_array = isLeapYear(year) ? days_in_months_leap_year : days_in_months_normal_year;

    int month = 0;
    while (days >= months_array[month]) {
        days -= months_array[month];
        month++;
    }

    const int zero_to_one_base = 1;
    time->year = year;
    time->month = month + zero_to_one_base;  // First month would be 0
    time->day = days + zero_to_one_base;     // First day would be 0
    return 0;
}