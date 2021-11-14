#ifndef TIMES_H_
 #define TIMES_H_

 extern uint8_t hour;
 extern uint8_t minute;
 extern uint8_t second;

 extern uint8_t day;

 extern uint16_t year;
 extern uint8_t month;
 extern uint8_t date;


 extern uint8_t alarm1_hour;
 extern uint8_t alarm2_hour;
 extern uint8_t alarm3_hour;

 extern uint8_t alarm1_minute;
 extern uint8_t alarm2_minute;
 extern uint8_t alarm3_minute;

 extern uint16_t alarm1Time;
 extern uint16_t alarm2Time;
 extern uint16_t alarm3Time;

 extern bool alarmUpdate_flag;



 void rtc_Init();
 void setTime();
 String readTime();
 void Load_AlarmTime();
#endif    