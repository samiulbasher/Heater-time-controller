/*
 * avrgpio.h
 *
 * Created: 17/02/2021 9:50:20 AM
 * Author: basher_samiul
*/

#ifndef _AVRGPIO_H
 #define _AVRGPIO_H

 #ifndef CBI
   //clear bit
   #define   CBI(sfr, bit)           (_SFR_BYTE(sfr) &= ~_BV(bit))
 #endif
 
 #ifndef SBI
   //set bit
   #define   SBI(sfr, bit)           (_SFR_BYTE(sfr) |= _BV(bit))
 #endif

#endif /* _AVRGPIO_H */