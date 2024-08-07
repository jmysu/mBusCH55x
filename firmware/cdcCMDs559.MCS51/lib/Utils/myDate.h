#ifndef _MYDATE_H
#define _MYDATE_H
 #ifdef _TITLE_STAMP_
  #include <stdint.h>
  #include <string.h>
  #include <stdlib.h> 
  #include <stdio.h>
  #include "ch559.h"    //GetCHipID
  #include "io.h"
  #include "usb/cdc_device.h"

  extern unsigned char _month, _day, _year;
  extern void getMyDate(void);
 
  extern void stampingTitle(const char *title);
  
 #endif
#endif