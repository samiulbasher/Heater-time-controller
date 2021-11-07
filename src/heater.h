#ifndef HEATER_H_
 #define HEATER_H_
 #include <Arduino.h>
 
 extern uint32_t heaterDisableTimerCounter;

 void heaterOn();
 void heaterOff();
 bool isHeaterOn();

#endif