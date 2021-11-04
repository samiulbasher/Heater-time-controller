#ifndef PINS_H__
 #define PINS_H__

    #define RX_PIN 0
    #define TX_PIN 1
    
    /*
    http://reprap.org/wiki/RAMPS_1.3/1.4_GADGETS3D_Shield_with_Panel
    It has a bunch of info on em, plus the pins for everything:

    BEEPER 37           // Beeper and is Connected into GADGETS3D shield MEGA_18BEEPER
    LCD_PINS_RS 16      // LCD control and is connected into GADGETS3D  shield LCDRS
    LCD_PINS_ENABLE 17  // LCD enable pin and is connected into GADGETS3D shield LCDE
    LCD_PINS_D4 23      // LCD signal pin, connected to Gadgets3D shield LCD4
    LCD_PINS_D5 25      // LCD signal pin, connected to Gadgets3D shield LCD5
    LCD_PINS_D6 27      // LCD signal pin, connected to Gadgets3D shield LCD6
    LCD_PINS_D7 29      // LCD signal pin, connected to Gadgets3D shield LCD7
    BTN_EN1 31          // Encoder left direction, connected to Gadgets3D shield S_E1
    BTN_EN2 33          // Encoder right direction, connected to Gadgets3D shield S_E2
    BTN_ENC 35          // Encoder Click, connected to Gadgets3D shield S_EC
    */

    /* LCD display pins */
    #define LCD_RS 16 
    #define LCD_EN 17
    #define LCD_D4 23
    #define LCD_D5 25
    #define LCD_D6 27
    #define LCD_D7 29

    /* Rotary encoder (dial) pins & encoder button pin */
    #define BTN_EN1 31 
    #define BTN_EN2 33
    #define BTN_ENC 35

    /* Buzzer  pin */
    #define BUZZER_IO 37

    /* Reset button pin */
    #define RESET_IO 41

    #define CLK 6
    #define DAT 5
    #define RST 4


 #endif    