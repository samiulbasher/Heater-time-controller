#include <Arduino.h>
#include <stdio.h>
#include <stdlib.h>

#include "DS1302.h"
#include "pins.h"


// Create a DS1302 object.
DS1302 rtc(CLK, DAT, RST);

uint8_t hour = 12;
uint8_t minute = 0;
uint8_t second = 0;

uint8_t day = 1;

uint16_t year = 2000;
uint8_t month = 1;
uint8_t date = 1;

const char *DAY[] = {NULL,"Sunday","Monday","Tuesday","Wednesday","Thursday","Friday","Saturday"};

void rtc_Init()
{
  // Initialize a new chip by turning off write protection and clearing the
  // clock halt flag. These methods needn't always be called. See the DS1302
  // datasheet for details.
  rtc.writeProtect(false);
  rtc.halt(false);

}


void setTime()
{
  // Make a new time object to set the date and time.
  Time t(year, month, date, hour, minute, second, day);
  // Set the time and date on the chip.
  rtc.time(t);
}

String readTime() {
  // Get the current time and date from the chip.
  Time t = rtc.time();

  hour = t.hr;
  minute =t.min;
  second = t.sec;

  day = t.day;
  //Serial.println(DAY[day]);

  year = t.yr;
  month = t.mon;
  date = t.date;

  // Name the day of the week.
  //const String day = dayAsString(t.day);

  // Format the time and date and insert into the temporary buffer.
  
  /*
  char buf[50];
  snprintf(buf, sizeof(buf), "%s %04d-%02d-%02d %02d:%02d:%02d",
           day.c_str(),
           t.yr, t.mon, t.date,
           t.hr, t.min, t.sec);
  */
  char buf[50];
  snprintf(buf, sizeof(buf), " %02d:%02d:%02d %02d/%02d ",
           t.hr, t.min, t.sec,
           t.mon, t.date);

  return String(buf);
  // Print the formatted string to serial so we can see the time.
}