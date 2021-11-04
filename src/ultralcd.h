#ifndef ULTRALCD_h
 #define ULTRALCD_h

 #include <Arduino.h>
 #include <stdlib.h>

 //#define REVERSE_ENCODER_DIRECTION

 #define BLEN_A 0
 #define BLEN_B 1
 #define BLEN_C 2
 #define EN_A (1<<BLEN_A)
 #define EN_B (1<<BLEN_B)
 #define EN_C (1<<BLEN_C)

 #define BTN_CLICKED (buttons & EN_C)

 #define blocktime 500 // blocking time for recognizing a new keypress of one key, ms

 //encoder rotation values
 #define encrot0          0
 #define encrot1          2
 #define encrot2          3
 #define encrot3          1
 

 #define ENCODER_PULSES_PER_STEP 2
 #define ENCODER_STEPS_PER_MENU_ITEM 4
 #define LCD_UPDATE_INTERVAL 1000
 #define LCD_CURSOR_UPDATE_INTERVAL 50
 #define LCD_TIMEOUT_TO_STATUS 15000

 #define LCD_WIDTH 16
 #define LCD_HEIGHT 2

 #define LONG_BEEP 0
 #define SHORT_BEEP 1

 #define HEATER_1_6MM 2
 #define HEATER_2_4MM 4

 enum SETTING_MENU {
    SET_DATE_TIME, 
    SET_ALARM,
    RESET_SET_ALARM
 }; 

 enum SET_DATE_TIME_MENU {
    BACK_MENU,
    SET_DAY,
    SET_TIME,
    SET_DATE
 };

 enum SET_ALARM_MENU {
    SET_ALARM_1,
    SET_ALARM_2,
    SET_ALARM_3
 };


 extern float homingOffset; //this variable read from eeprom
 extern uint8_t cutting_delay; //this variable read from eeprom
 extern uint8_t update_eeprom;
 extern float previousHomingOffset; 
 extern int8_t heaterTypeSelect;//this variable read from eeprom
 extern int8_t heaterTypeSection;


 void rotaryEncoder_Init();
 void lcd_Init();
 void rotary_buttons_update();
 void lcd_update();
 void lcd_clear();
 void beeper(uint8_t numOfBeep, uint8_t beepPattern);
 void menu_update();
 void statusUpdate();
  
 bool isButtonPressed();
 void buttonReleased();

 void settingMenu();
 void setDateTimeMenu();
 void setAlarmMenu();

 void waitingTime(float s);
 char* jp(const char *text);

 void printStr(String text);
 void printStr(String text, uint8_t index, uint8_t line);
 void printChar(uint8_t c, uint8_t index, uint8_t line);
 void printInt(uint16_t value, uint8_t index, uint8_t line);
 void printFloat(float value, uint8_t index, uint8_t line);

    // //Custom characters
    // byte chr_arrowleft[8] = {
    // 	0x00, 0x04, 0x08, 0x1F,
    // 	0x08, 0x04, 0x00, 0x00
    // };
    // byte chr_arrowright[8] = {
    // 	0x00, 0x04, 0x02, 0x1F,
    // 	0x02, 0x04, 0x00, 0x00
    // };
    // byte chr_arrowup[8] = {
    // 	0x00, 0x04, 0x0E, 0x15,
    // 	0x04, 0x04, 0x00, 0x00
    // };
    // byte chr_arrowdown[8] = {
    // 	0x00, 0x04, 0x04, 0x15,
    // 	0x0E, 0x04, 0x00, 0x00
    // };
    // byte chr_slash[8] = {
    // 	0x00, 0x01, 0x02, 0x04,
    // 	0x08, 0x10, 0x00, 0x00
    // };
    // byte chr_indleft[8] = {
    // 	0x00, 0x04, 0x08, 0x10,
    // 	0x08, 0x04, 0x00, 0x00
    // };
    // byte chr_indright[8] = {
    // 	0x00, 0x04, 0x02, 0x01,
    // 	0x02, 0x04, 0x00, 0x00
    // };
    // byte chr_plus[8] = {
    // 	0x00, 0x04, 0x04, 0x1F,
    // 	0x04, 0x04, 0x00, 0x00
    // };
    // byte chr_minus[8] = {
    // 	0x00, 0x00, 0x00, 0x1F,
    // 	0x00, 0x00, 0x00, 0x00
    // };


    // /*
    // 	Clears a desired display line
    // 	line: int > Line to clear
    // */
    // void clearLine(int line) {
    // 	lcd.setCursor(0, line);
    // 	lcd.print("                ");
    // }

#endif
