#include <Arduino.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdlib.h>

#include "pins.h"
#include "config.h"
#include "avrgpio.h"
#include "ultralcd.h"
#include "DS1302.h"
#include "times.h"


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

  readTime();
  setTime();
}


void loop() {
  // put your main code here, to run repeatedly:
  rotary_buttons_update();
	lcd_update();

  // readTime();
  // delay(1000);
}

