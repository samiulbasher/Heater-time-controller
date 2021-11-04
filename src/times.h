#ifndef TIMES_H_
 #define TIMES_H_

 extern uint8_t hour;
 extern uint8_t minute;
 extern uint8_t second;

 extern uint8_t day;

 extern uint16_t year;
 extern uint8_t month;
 extern uint8_t date;


 void rtc_Init();
 void setTime();
 String readTime();
#endif    