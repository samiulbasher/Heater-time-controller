#include "EEPROM.h"
#if ARDUINO < 100
#include <WProgram.h>
#else
#include <Arduino.h>
#endif

#include <string.h>

#include "config.h"
#include "command.h"
#include "ultralcd.h"
#include "times.h"

int eepromAddr = 0; // this is the byte where the EEPROM functions will start writing
//RAM info
extern unsigned int __bss_end;
extern unsigned int __heap_start;
extern void *__brkval;


// EEPROM functions for handling more than byte size types, see http://www.arduino.cc/cgi-bin/yabb2/YaBB.pl?num=1234477290
template <class T> int EEPROM_writeAnything(int &ee, const T& value)
{
  const byte* p = (const byte*)(const void*)&value;
  int i;
  for (i = 0; i < (int)sizeof(value); i++)
    EEPROM.write(ee++, *p++);
  return i;
}

template <class T> int EEPROM_readAnything(int &ee, T& value)
{
  byte* p = (byte*)(void*)&value;
  int i;
  for (i = 0; i < (int)sizeof(value); i++)
    *p++ = EEPROM.read(ee++);
  return i;
}

void Save_eeprom()  {
  int addr = eepromAddr;
  addr += EEPROM_writeAnything(addr, alarm1_hour);     //"alarm1_hour" declared as a extern variable in ultralcd.h
  addr += EEPROM_writeAnything(addr, alarm1_minute);   //"alarm1_minute" declared as a extern variable in ultralcd.h
  addr += EEPROM_writeAnything(addr, alarm1_OnOff_Flag);

  addr += EEPROM_writeAnything(addr, alarm2_hour);     //"alarm2_hour" declared as a extern variable in ultralcd.h
  addr += EEPROM_writeAnything(addr, alarm2_minute);   //"alarm2_minute" declared as a extern variable in ultralcd.h
  addr += EEPROM_writeAnything(addr, alarm2_OnOff_Flag);

  addr += EEPROM_writeAnything(addr, alarm3_hour);     //"alarm3_hour" declared as a extern variable in ultralcd.h
  addr += EEPROM_writeAnything(addr, alarm3_minute);   //"alarm3_minute" declared as a extern variable in ultralcd.h
  addr += EEPROM_writeAnything(addr, alarm3_OnOff_Flag);


  Serial.println("Saving data to the eeprom...");
}

void Load_eeprom()  {
  int addr = eepromAddr;
  addr += EEPROM_readAnything(addr, alarm1_hour);
  if(isnan(alarm1_hour)) {  alarm1_hour = 0;  }
  addr += EEPROM_readAnything(addr, alarm1_minute);
  if(isnan(alarm1_minute)) { alarm1_minute = 0; }
  addr += EEPROM_readAnything(addr, alarm1_OnOff_Flag);
  if(isnan(alarm1_OnOff_Flag)) { alarm1_OnOff_Flag = 0; }

  addr += EEPROM_readAnything(addr, alarm2_hour);
  if(isnan(alarm2_hour)) { alarm2_hour = 0; }
  addr += EEPROM_readAnything(addr, alarm2_minute);
  if(isnan(alarm2_minute)) { alarm2_minute = 0; }
  addr += EEPROM_readAnything(addr, alarm2_OnOff_Flag);
  if(isnan(alarm2_OnOff_Flag)) { alarm2_OnOff_Flag = 0; }

  addr += EEPROM_readAnything(addr, alarm3_hour);
  if(isnan(alarm3_hour)) { alarm3_hour = 0; }
  addr += EEPROM_readAnything(addr, alarm3_minute);
  if(isnan(alarm3_minute)) { alarm3_minute = 0; }
  addr += EEPROM_readAnything(addr, alarm3_OnOff_Flag);
  if(isnan(alarm3_OnOff_Flag)) { alarm3_OnOff_Flag = 0; }


  //Serial.println(alarm1_hour);
  //Serial.println(alarm1_minute);
  //Serial.println(alarm2_hour);
  //Serial.println(alarm2_minute);
  ///Serial.println(alarm3_hour);
  //Serial.println(alarm3_minute);
}


int freeMemory() {
  int free_memory;

  if((int)__brkval == 0)
    free_memory = ((int)&free_memory) - ((int)&__bss_end);
  else
    free_memory = ((int)&free_memory) - ((int)__brkval);

  return free_memory;
}