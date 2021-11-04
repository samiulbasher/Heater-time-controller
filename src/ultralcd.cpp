#include <avr/pgmspace.h>
#include <Wire.h>
#include <LiquidCrystal.h>
#include <EEPROM.h>
#include <math.h>

#include "ultralcd.h"
#include "pins.h"
#include "config.h"
#include "DS1302.h"
#include "times.h"

LiquidCrystal lcd(LCD_RS, LCD_EN, LCD_D4, LCD_D5, LCD_D6, LCD_D7);

char jpconv[64]; //english to japanese conversion

int8_t incrementalMode = 0;
int8_t temp_incrementalMode = 0;
int8_t heaterTypeSection = 0; 

void lcdProgMemprint(const char *str)
{
  char ch=pgm_read_byte(str);
  while(ch)
  {
    lcd.print(ch);
    ch=pgm_read_byte(++str);
  }
}
#define lcdprintPGM(x) lcdProgMemprint(PSTR(x))

uint8_t arrow[8] = {B00000, B00100, B00110 ,B11111, B00110, B00100, B00000, B00000};
uint8_t back[8]  = {B00100, B01110 ,B11111, B00100, B11100, B00000, B00000, B00000};
uint8_t down[8]  = {B00000, B00000 ,B00000, B00000, B00000, B10001, B01010, B00100};

#define ARROW 0
#define BACK  1
#define DOWN  2


uint8_t buttonState = 0;
uint8_t last_buttonState = 0;
uint8_t buttonPressed = 0;

uint32_t blocking_enc;
uint32_t lcd_next_update_millis;
uint32_t lcd_cursor_update_millis;
int8_t encoderpos; 
uint32_t encoderPosition;
uint32_t lastEncoderPosition = 1;
uint8_t lastEncoderBits;


uint32_t selection_jumping_millis;

volatile uint8_t buttons; //Contains the bits of the currently pressed buttons.
uint32_t previous_millis_cursor = 0;

uint8_t buttonPressedCount= 0;

int8_t index = 0;
int8_t lastIndex = 0;

//rotaryIncrement    
int8_t rotaryIncrement= 0;
int8_t last_rotaryIncrement = 0;

int8_t currentPage = 0;

SETTING_MENU settingMenuItem; //main manu
uint8_t totalSettingMenuItem = (RESET_SET_ALARM - SET_DATE_TIME); //last menu item - fast menu item 

SET_DATE_TIME_MENU dateTimeMenuItem; 
uint8_t totalDateTimeMenuItem = (SET_DATE - DATE_TIME_BACK_MENU);

SET_ALARM_MENU alarmMenuItem;
uint8_t totalAlarmMenuItem = (SET_ALARM_3 - ALARM_BACK_MENU);

uint8_t daySection_Counter = 0;
uint8_t timeSecltion_Counter = 0;
uint16_t dateSecltion_Counter = 0;

float temp_homingOffset = 0;


bool statusFlag = true;
bool settingMenuFlag = true;
bool dateTimeMenuFlag =false;
bool alarmMenuFlag =false;

bool manuallySelectSettingMenuMenu = false;
bool manuallySelectDateTimeMenu = false;
bool manuallySelectAlarmMenu = false;

void rotaryEncoder_Init()
{
  pinMode(BUZZER_IO, OUTPUT);
  pinMode(BTN_EN1, INPUT_PULLUP);
  pinMode(BTN_EN2, INPUT_PULLUP);
  pinMode(BTN_ENC, INPUT_PULLUP);

  rotary_buttons_update();
  encoderpos = 0;
}

void beeper(uint8_t numOfBeep, uint8_t beepPattern)
{
  uint16_t beepDelay;
  
  if(beepPattern == SHORT_BEEP)  beepDelay = 10;
  if(beepPattern == LONG_BEEP )  beepDelay = 300;

  for (int i = 0; i < numOfBeep; i++)
  {
    digitalWrite(BUZZER_IO, HIGH);
    delay(beepDelay);
    digitalWrite(BUZZER_IO, LOW);
    delay(beepDelay);
  }
}


void lcd_Init()
{
  lcd.begin(LCD_WIDTH, LCD_HEIGHT);
  lcd.createChar(ARROW, arrow);   //Create the arrow symbol
  lcd.createChar(BACK, back);     //Create the back symbol
  lcd.createChar(DOWN, down );    //Create the back symbol
  lcd.home();
  lcd.clear();
}

void lcd_clear()
{
  lcd.home();
  lcd.clear();
}


void lcd_update()
{
  if (lcd_cursor_update_millis < millis())
  {
    if (abs(encoderpos) >= ENCODER_PULSES_PER_STEP)
    {
      encoderPosition += encoderpos / ENCODER_PULSES_PER_STEP;
      index = encoderPosition / ENCODER_STEPS_PER_MENU_ITEM;

      if(index > lastIndex) 
        rotaryIncrement ++;
      else if(index < lastIndex) 
        rotaryIncrement --;  
      //Serial.println(rotaryIncrement);

      encoderpos = 0;
      lastIndex = index;
    }
    
    //satus menu selection
    if(statusFlag)        
    { 
      currentPage = 0; //reset
      statusUpdate(); 
      if(buttonPressed) // go to submenu
      {
        buttonReleased();
        beeper(1,SHORT_BEEP);
        statusFlag = false; 
      }
    }
    // manu selection
    else if(!statusFlag)  
    { 
      menu_update();  
    }

    lcd_cursor_update_millis = millis() + 100;  
  }
  isButtonPressed(); //check if button pressed por not
}

bool isButtonPressed()
{
  if(BTN_CLICKED) 
    buttonPressed = true;
  return buttonPressed;
}

void buttonReleased()
{
  buttonPressed = false;
}

void statusUpdate()
{
  if (lcd_next_update_millis < millis())
  {
    printStr(readTime(), 0, 0);
    printStr( "NEXT T.ON: 12:00", 0, 1);

    lcd_next_update_millis = millis() + LCD_UPDATE_INTERVAL; 
  }
}


void rotary_buttons_update()
{
  uint8_t newbutton=0;
  if(digitalRead(BTN_EN1)==0)  newbutton|=EN_A;
  if(digitalRead(BTN_EN2)==0)  newbutton|=EN_B;
  
  uint8_t buttonState = 0;
  buttonState = !digitalRead(BTN_ENC);
  if((blocking_enc<millis()) && (last_buttonState != buttonState))  
  {
    if(last_buttonState) 
    {
      newbutton |= EN_C; 
      last_buttonState = 0;
      blocking_enc = millis() + blocktime;
    }
    else
      last_buttonState = 1;
  }
  buttons = newbutton;

  // if((blocking_enc<millis()) && (digitalRead(BTN_ENC)==0))  
  //   newbutton |= EN_C;
  // buttons = newbutton;
  // // Serial.println(buttons,BIN);

  
  #ifdef REVERSE_ENCODER_DIRECTION
    #define ENCODER_DIFF_CW  (encoderpos--)
    #define ENCODER_DIFF_CCW (encoderpos++)
  #else
    #define ENCODER_DIFF_CW  (encoderpos++)
    #define ENCODER_DIFF_CCW (encoderpos--)  
  #endif
  
  #define ENCODER_SPIN(_E1, _E2) switch (lastEncoderBits) { case _E1: ENCODER_DIFF_CW; break; case _E2: ENCODER_DIFF_CCW; }
    
 //manage encoder rotation
  uint8_t enc=0;
  if (buttons & EN_A) enc |= B01;
  if (buttons & EN_B) enc |= B10;
  if(enc != lastEncoderBits)
  {
    switch (enc) 
    {
        case encrot0: ENCODER_SPIN(encrot3, encrot1); break;
        case encrot1: ENCODER_SPIN(encrot0, encrot2); break;
        case encrot2: ENCODER_SPIN(encrot1, encrot3); break;
        case encrot3: ENCODER_SPIN(encrot2, encrot0); break;
    }
  }
  lastEncoderBits = enc;
}


void menu_update()
{
  /*------------------cursor selection------------------*/
  if(rotaryIncrement != last_rotaryIncrement &&  millis() - previous_millis_cursor >= LCD_CURSOR_UPDATE_INTERVAL)
  {
    if(rotaryIncrement > last_rotaryIncrement)
    {
      previous_millis_cursor=millis();
      currentPage++;

      if(settingMenuFlag && currentPage >= totalSettingMenuItem)
        currentPage = totalSettingMenuItem;
      if(dateTimeMenuFlag && currentPage >= totalDateTimeMenuItem)
        currentPage = totalDateTimeMenuItem;
      if(alarmMenuFlag && currentPage >= totalAlarmMenuItem)
        currentPage = totalAlarmMenuItem;
    }

    else if(rotaryIncrement < last_rotaryIncrement)
    {
      previous_millis_cursor=millis();
      currentPage--;

      if (currentPage <= 0)
      currentPage = 0;
    }

    // -------------debug---------------
    if(DEBUGGING)
    {
      Serial.print("RotaryIncrement: ");
      Serial.print(rotaryIncrement    );
      Serial.print("    CurrentPage: ");
      Serial.println(currentPage);
    }
  }
  /*-----------------------end--------------------------*/

  /*------------------menu selection--------------------*/
  if (settingMenuFlag) //main manu selection          
  {
    if(manuallySelectSettingMenuMenu) //if inside the menu, do not turn rotaryIncrement    
    {
      currentPage = settingMenuItem;
      manuallySelectSettingMenuMenu = false;
      manuallySelectDateTimeMenu = false;
      manuallySelectAlarmMenu = false;
    }

    settingMenuItem = (SETTING_MENU)currentPage;
    settingMenu();
  }

  else if (dateTimeMenuFlag) //submenu selection 
  {
    if(manuallySelectDateTimeMenu)// if inside the submenu, do not turn rotaryIncrement    
    {
      currentPage = dateTimeMenuItem;
      manuallySelectSettingMenuMenu = false;
      manuallySelectDateTimeMenu = false;
      manuallySelectAlarmMenu = false;
    }

    dateTimeMenuItem = (SET_DATE_TIME_MENU)currentPage;
    setDateTimeMenu();
  }

  else if (alarmMenuFlag) //submenu1 selection 
  {
    if(manuallySelectAlarmMenu)// if inside the submenu, do not turn rotaryIncrement    
    {
      currentPage = alarmMenuItem;
      manuallySelectSettingMenuMenu = false;
      manuallySelectDateTimeMenu = false;
      manuallySelectAlarmMenu = false;
    }
    
    alarmMenuItem = (SET_ALARM_MENU)currentPage;
    setAlarmMenu();
  }
  /*-----------------------end--------------------------*/
  last_rotaryIncrement = rotaryIncrement;
  lastEncoderPosition = encoderPosition;
}


void settingMenu()
{
  switch(settingMenuItem)
  { 
    case SET_DATE_TIME:
    { 
      printStr(jp("      MENU      "), 0, 0); 
      printChar(ARROW, 0, 1);
      printStr(jp( "Set date time ^"));   
      printChar(DOWN, 15, 1);       
      
      if(buttonPressed) 
      {
        buttonReleased();
        beeper(1,SHORT_BEEP);
        settingMenuFlag = false;
        dateTimeMenuFlag = true; 
        alarmMenuFlag = false; 
        manuallySelectDateTimeMenu = true; 
        dateTimeMenuItem = (SET_DATE_TIME_MENU) SET_DAY;
      }
    }
    break;

    case SET_ALARM:
    {
      printStr(jp(" Set date time ^"), 0, 0);   
      printChar(ARROW, 0, 1);
      printStr(jp( "Set alarm      "));
      printChar(DOWN, 15, 1);   
      if(buttonPressed)
      {
        buttonReleased();
        beeper(1,SHORT_BEEP);
        settingMenuFlag = false;
        dateTimeMenuFlag = false;  
        alarmMenuFlag = true; 
        manuallySelectAlarmMenu = true;
        alarmMenuItem = (SET_ALARM_MENU) SET_ALARM_1;
      }
    }
    break;

    case RESET_SET_ALARM:
    {
      printStr(jp(" Set alarm     ^"), 0, 0);   
      printChar(ARROW, 0, 1);
      printStr(jp( "Reset alarm    "));
  
      if(buttonPressed)
      {
        
        buttonReleased();
        beeper(1,SHORT_BEEP);
        statusFlag = true;
        // settingMenuFlag = false;
        // dateTimeMenuFlag = false;  
        // alarmMenuFlag = true; 
        // manuallySelectAlarmMenu = true;
      }
    }
    break;
  }
}

void setDateTimeMenu()
{
  switch(dateTimeMenuItem)
  { 
    case DATE_TIME_BACK_MENU:
    { 
      printChar(BACK, 0, 0);
      printStr(jp(" SET DATE TIME "));   
      printStr(jp(" Set Day       "), 0 ,1);
      printChar(DOWN, 15, 1);   
      if(buttonPressed) // go to submenu
      {
        buttonReleased();
        beeper(1,SHORT_BEEP);
        statusFlag = true;
        settingMenuFlag = true;
      }
    }
    break;

    case SET_DAY:
    { 
      uint8_t select = 0;
      if (buttonPressedCount == 1) 
      {
        select = 1;
        manuallySelectDateTimeMenu = true;
      }

      if(select == 0)
      {
        printChar(ARROW, 0, 0);
        printStr(jp( "Set Day       ^"));   
        printStr(jp(" Set Time       "), 0 ,1);
        printChar(DOWN, 15, 1);   
      }
      else
      {
        printStr(jp("   SELECT DAY   "), 0 ,0);

        if(encoderPosition > lastEncoderPosition && daySection_Counter < 7)   { daySection_Counter +=1; }
        if(encoderPosition < lastEncoderPosition && daySection_Counter > 1)   { daySection_Counter -=1; }

        switch (daySection_Counter)
        {
          case 1:
             printChar(ARROW, 0, 1);
             printStr(jp("Sunday          "));
            break;
          case 2:
            printChar(ARROW, 0, 1);
             printStr(jp("Monday          "));
            break;
          case 3:
             printChar(ARROW, 0, 1);
             printStr(jp("Tuesday         "));
            break;
          case 4:
             printChar(ARROW, 0, 1);
             printStr(jp("Wednesday       "));
            break;
          case 5:
             printChar(ARROW, 0, 1);
             printStr(jp("Thursday        "));
            break;
          case 6:
             printChar(ARROW, 0, 1);
             printStr(jp("Friday          "));
            break;
          case 7:
             printChar(ARROW, 0, 1);
             printStr(jp("Saturday        "));
            break;
        }   
      }
      if(buttonPressed) // go to submenu
      {
        buttonReleased();
        beeper(1,SHORT_BEEP);
        buttonPressedCount++;
  
        if(buttonPressedCount >= 2)
        {
          buttonPressedCount = 0;
          day = daySection_Counter;
          Serial.println(day);
          setTime(); //update date time
          dateTimeMenuFlag = true; 
          manuallySelectDateTimeMenu = true; 
          dateTimeMenuItem = (SET_DATE_TIME_MENU) SET_TIME;
        }
        daySection_Counter = day;
      }
    }
    break;

    case SET_TIME:
    { 
      uint8_t select = 0;
      if (buttonPressedCount != 0) 
      {
        select = 1;
        manuallySelectDateTimeMenu = true;
      }
      
      if(select == 0)
      {
        printStr(jp(" Set Day       ^"), 0 ,0);   
        printChar(ARROW, 0, 1);
        printStr(jp( "Set Time       "));
        printChar(DOWN, 15, 1);  
      } 
      else
      {
        printStr(jp("  SELECT TIME   "), 0 ,0);

        if(encoderPosition > lastEncoderPosition )   { timeSecltion_Counter +=1; }
        if(encoderPosition < lastEncoderPosition )   { timeSecltion_Counter -=1; }

        if(buttonPressedCount == 1) // for hour
        {
          if(timeSecltion_Counter > 23)
           timeSecltion_Counter = 23;
          if(timeSecltion_Counter < 0)
           timeSecltion_Counter = 0;
        }
        else // for min and sec
        {
          if(timeSecltion_Counter > 59)
           timeSecltion_Counter = 59;
          if(timeSecltion_Counter < 0)
           timeSecltion_Counter = 0;
        }
      
        char tempTime[10];
        snprintf(tempTime, sizeof(tempTime), "%02d:%02d:%02d", hour, minute, second);

        switch(buttonPressedCount)
        {
          case 1:
             if(selection_jumping_millis < millis())
             {
              printStr(jp("  "), 3 ,1);
              selection_jumping_millis = millis() + 500; 
             }
             else
              printStr(jp(tempTime), 3 ,1);
             hour = timeSecltion_Counter; 
            break; 
          case 2:
             if(selection_jumping_millis < millis())
             {
              printStr(jp("  "), 6 ,1);
              selection_jumping_millis = millis() + 500; 
             }
             else
              printStr(jp(tempTime), 3 ,1);
             minute = timeSecltion_Counter; 
            break; 
          case 3:
             if(selection_jumping_millis < millis())
             {
              printStr(jp("  "), 9 ,1);
              selection_jumping_millis = millis() + 500; 
             }
             else
              printStr(jp(tempTime), 3 ,1);
             second = timeSecltion_Counter; 
            break; 
        }
      }

      if(buttonPressed) 
      {
        buttonReleased();
        beeper(1,SHORT_BEEP);
        buttonPressedCount++;
        if(buttonPressedCount == 1) // hour select
        {
          lcd.clear();
          timeSecltion_Counter = hour;
        }
        if(buttonPressedCount == 2) // min select
        {
          //lcd.clear();
          timeSecltion_Counter = minute;
        }
        if(buttonPressedCount == 3) // sec select
        {
          //lcd.clear();
          timeSecltion_Counter = second;
        }
        if(buttonPressedCount == 4) // end 
        {
          buttonPressedCount=0;
          setTime(); //update date time
          dateTimeMenuFlag = true; 
          manuallySelectDateTimeMenu = true; 
          dateTimeMenuItem = (SET_DATE_TIME_MENU) SET_DATE;
        }
      }
    }
    break;
    
    case SET_DATE:
    { 
           uint8_t select = 0;
      if (buttonPressedCount != 0) 
      {
        select = 1;
        manuallySelectDateTimeMenu = true;
      }
      
      if(select == 0)
      {
        printStr(jp(" Set Time      ^"), 0 ,0);   
        printChar(ARROW, 0, 1);
        printStr(jp( "Set Date       "));
      }
      else
      {
        printStr(jp("  SELECT DATE   "), 0 ,0);

        if(encoderPosition > lastEncoderPosition )   { dateSecltion_Counter +=1; }
        if(encoderPosition < lastEncoderPosition )   { dateSecltion_Counter -=1; }

        if(buttonPressedCount == 1) // for year
        {
          if(dateSecltion_Counter > 2099)
           dateSecltion_Counter = 2099;
          if(dateSecltion_Counter < 2000)
           dateSecltion_Counter = 2000;
        }
        if(buttonPressedCount == 2) // for month
        {
          if(dateSecltion_Counter > 12)
           dateSecltion_Counter = 12;
          if(dateSecltion_Counter < 1)
           dateSecltion_Counter = 1;
        }
        if(buttonPressedCount == 3) // for date
        {
          if(dateSecltion_Counter > 31)
           dateSecltion_Counter = 31;
          if(dateSecltion_Counter < 1)
           dateSecltion_Counter = 1;
        }

        char tempDate[12];
        snprintf(tempDate, sizeof(tempDate), "%04d/%02d/%02d", year, month, date);

        switch(buttonPressedCount)
        {
          case 1:
             if(selection_jumping_millis < millis())
             {
              printStr(jp("    "), 3 ,1);
              selection_jumping_millis = millis() + 500; 
             }
             else
              printStr(jp(tempDate), 3,1);
             year = dateSecltion_Counter; 
            break; 
          case 2:
             if(selection_jumping_millis < millis())
             {
              printStr(jp("  "), 8 ,1);
              selection_jumping_millis = millis() + 500; 
             }
             else
              printStr(jp(tempDate), 3 ,1);
             month = dateSecltion_Counter; 
            break; 
          case 3:
             if(selection_jumping_millis < millis())
             {
              printStr(jp("  "), 11 ,1);
              selection_jumping_millis = millis() + 500; 
             }
             else
              printStr(jp(tempDate), 3 ,1);
             date = dateSecltion_Counter; 
            break; 
        }
      }

      if(buttonPressed) 
      {
        buttonReleased();
        beeper(1,SHORT_BEEP);
        buttonPressedCount++;
        if(buttonPressedCount == 1) // year select
        {
          lcd.clear();
          dateSecltion_Counter = year;
        }
        if(buttonPressedCount == 2) // month select
        {
          //lcd.clear();
          dateSecltion_Counter = month;
        }
        if(buttonPressedCount == 3) // date select
        {
          //lcd.clear();
          dateSecltion_Counter = date;
        }
        if(buttonPressedCount == 4) // end 
        {
          buttonPressedCount=0;
          setTime(); //update date time
          statusFlag = true;
          settingMenuFlag = true;
          dateTimeMenuFlag = false; 
        }
      }
    }
    break;
  }
}

void setAlarmMenu()
{
  switch(alarmMenuItem)
  { 
    case ALARM_BACK_MENU:
    {
      printChar(BACK, 0, 0);
      printStr(jp("   SET ALARM   "));   
      printStr(jp(" Set Alarm 1   "), 0 ,1);
      printChar(DOWN, 15, 1);   
      if(buttonPressed) // go to submenu
      {
        buttonReleased();
        beeper(1,SHORT_BEEP);
        statusFlag = true;
        settingMenuFlag = true;
      }
    }
    break;
    case SET_ALARM_1:
    { 
      printChar(ARROW, 0, 0);
      printStr(jp( "Set Alarm 1   ^"));   
      printStr(jp(" Set Alarm 2    "), 0 ,1);
      printChar(DOWN, 15, 1);   
      if(buttonPressed) // go to submenu
      {
        buttonReleased();
        beeper(1,SHORT_BEEP);
        //statusFlag = true;
        //settingMenuFlag = true;
      }
    }
    break;

    case SET_ALARM_2:
    { 
      printStr(jp(" Set Alarm 1   ^"), 0 ,0);   
      printChar(ARROW, 0, 1);
      printStr(jp( "Set Alarm 2    "));
      printChar(DOWN, 15, 1);  
      if(buttonPressed) // go to submenu
      {
        buttonReleased();
        beeper(1,SHORT_BEEP);
        //statusFlag = true;
        //settingMenuFlag = true;
      } 
    }
    break;

    case SET_ALARM_3:
    { 
      printStr(jp(" Set Alarm 2   ^"), 0 ,0);   
      printChar(ARROW, 0, 1);
      printStr(jp( "Set Alarm 3    ")); 
      if(buttonPressed) // go to submenu
      {
        buttonReleased();
        beeper(1,SHORT_BEEP);
        //statusFlag = true;
        //settingMenuFlag = true;
      }
    }
    break;
  }
}



/*
	Prints text to display
	desc > Will wrap to second display line
		   if text is bigger than 20 chars.
		   Doesn't work for text with more
		   than 32 chars (lcd resets).
	text: String > Text to print
	index: int > Index to print text at
	line: int > Line to print text at
*/
void printStr(String text, uint8_t index, uint8_t line) 
{
	if(text.length() > LCD_WIDTH) 
  {
		lcd.setCursor(0, 0);
		lcd.print(text);
		lcd.setCursor(0, 1);
		lcd.print(text.substring(LCD_WIDTH + 1, text.length()));
	}
	else 
  {
		lcd.setCursor(index, line);
		lcd.print(text);
	}
}

void printChar(uint8_t c, uint8_t index, uint8_t line)
{
  lcd.setCursor(index, line);
  lcd.write(uint8_t(c));
}

void printStr(String text)
{
  lcd.print(text);
}

void printInt(uint16_t value, uint8_t index, uint8_t line)
{
  lcd.setCursor(index, line);
  lcd.print(value);
}

void printFloat(float value, uint8_t index, uint8_t line)
{
  lcd.setCursor(index, line);
  lcd.print(value, 2);
}

/*
* custom delay function
*/
void waitingTime(float s)
{
	int16_t ms = s*1000;
	while (0 < ms)
	{
		delay(1); //1ms delay
		--ms;
	}
}

/*
* This code converts half-width Katakana from Unicode
* https://programresource.net/2020/03/09/3113.html
* https://synapse.kyoto/lib/MGLCD/CharacterCode/page001.html
*/
char* jp(const char *text) 
{
  char *cur = &jpconv[0];
  const char *chk = text;
  while (*chk != 0) 
  {
    if ((*chk & 0xFF) == 0xEF && (*(chk+1) & 0xBC) == 0xBC) 
    {
      *cur = (*++chk & 0x02) << 5;
      *cur++ = *++chk + *cur;
    } 
    else
      *cur++ = *chk;
    chk++;
  }
  *cur = 0;
  return jpconv;
}
 
// -------------------------------------- End --------------------------------------