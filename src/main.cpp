#include <Arduino.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdlib.h>

#include "pins.h"
#include "config.h"
#include "avrgpio.h"
#include "ultralcd.h"
#include "heater.h"
#include "command.h"
#include "DS1302.h"
#include "times.h"

uint8_t update_eeprom = false;
uint32_t heaterDisableTimerCounter = 0;


void setup() 
{
  Serial.begin(BAUDRATE);

  lcd_Init();
  rotaryEncoder_Init();

  rtc_Init();

  printStr(" Heater Tm.Ctrl ", 0, 0);
  printStr("Firmware Ver:___", 0, 1);
  printFloat(FIRMWARE_VERSION, 13, 2);
  
  delay(2000);
  lcd_clear();

  Load_eeprom(); //load data from eeprom
  readTime(); //read time from DS1302
}


void loop() {
  // put your main code here, to run repeatedly:
  rotary_buttons_update();
	lcd_update();

  if(update_eeprom){
	 update_eeprom = false;
	 Save_eeprom();
	}

  if(isHeaterOn())
  {
    //After turning on the heater, the heater will turn off within 30 minutes
    if(millis() > heaterDisableTimerCounter + 1000L* 60 * 2) 
	  {
      heaterOff();
    }
  }
}

