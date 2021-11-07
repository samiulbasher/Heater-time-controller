#include <Arduino.h>
#include "heater.h"
#include "pins.h"
#include "ultralcd.h"

uint8_t heaterOnOff_Flag = false;

void heaterOn()
{  
   beeper(1,LONG_BEEP);
   heaterOnOff_Flag = true;
   heaterDisableTimerCounter = millis();
   digitalWrite(HEATER_ON_PIN, HIGH);
}

void heaterOff()
{
   beeper(1,LONG_BEEP);
   heaterOnOff_Flag = false;
   digitalWrite(HEATER_ON_PIN, LOW);
}

bool isHeaterOn()
{
   bool rtn = false;
   if(heaterOnOff_Flag)
    rtn = true;

   return rtn;
}