#include <Arduino.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdlib.h>

#include "pins.h"
#include "config.h"
#include "avrgpio.h"
#include "ultralcd.h"
#include "command.h"
#include "DS1302.h"
#include "times.h"

uint8_t update_eeprom = false;
uint32_t heaterDisableTimerCounter = 0;

uint8_t heaterOnFlag = false;

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
  //alarm 1 hour and min
	if(isnan(alarm1_hour)) {
	 alarm1_hour = 0;
	}
	if(isnan(alarm1_minute)) {
	 alarm1_minute = 0;
	}
  
  //alarm 2 hour and min
	if(isnan(alarm2_hour)) {
	 alarm2_hour = 0;
	}
	if(isnan(alarm2_minute)) {
	 alarm2_minute = 0;
	}

  //alarm 3 hour and min
  if(isnan(alarm3_hour)) {
	 alarm3_hour = 0;
	}
	if(isnan(alarm3_minute)) {
	 alarm3_minute = 0;
	}

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

  if(heaterOnFlag)
  {
    //After turning on the heater, the heater will turn off within 30 minutes
    if(millis() > heaterDisableTimerCounter + 1000L* 60 * 30) 
	  {
      heaterOnFlag = false;
      digitalWrite(HEATER_ON_PIN, LOW);
    }
  }
}

