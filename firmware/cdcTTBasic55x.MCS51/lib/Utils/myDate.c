/*         
 * myDate.c
 *
 *         01234567890                  
 *    Date Jul 11 2024  
 * 
 * */
#include "myDate.h"
#include "ansiColor.h"
#include "io.h" 
#include <stdio.h>
#include <dataflash.h>

#ifdef _TITLE_STAMP_
unsigned char _month, _day, _year;
const char *months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", 
                        "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

void getMyDate(void)
{
  char temp [] = __DATE__;
  unsigned char i;

  _year = atoi(temp + 9);
  *(temp + 6) = 0;
  _day = atoi(temp + 4);
  *(temp + 3) = 0;
  for (i = 0; i < 12; i++) {    
    if (!strcmp(temp, months[i])) {
      _month = i + 1;
      return;
      }
    }
}

#define CHIP_ID SAFE_MOD
void stampingTitle(const char *title) {
  printf("\033[2J"); //Clear all
  delay(100); //TeraTerm NEED 100ms to clear screen!!!
  //printf("\033[2J\033[H"); //Clear all, home       
  printf(ANSI_CLR_HOME);   // Clear screen

  printf("\033[" ANSI_GREEN "m");                                                     // Green
  printf(__TIMESTAMP__);                                                          // Build timestamp
  printf(" CH5%02X:%08X\n\r", CHIP_ID, GetChipID());  // Chip ID
  printf("\033[" ANSI_YELLOW "m");                                                    // Yellow
  getMyDate();                                                                        // Get date in number from __DATE__
  printf("\033[1m");      //Bold    
                                                     
  //printf("___%s.%02u%02u%02u___ [%02X-%02X]\n\r", title, 
  //  _year,_month,_day,
  //  dataflash_at(0xFFFE), dataflash_at(0xFFFF));  //Flash Config   
    
  uint16_t idMCU;
  __code unsigned char* data_flash_FFFE = (__code uint8_t*)0xFFFE;
  memcpy(&idMCU, data_flash_FFFE, 2);
  printf("___%s.%02u%02u%02u___ [%04X]\n\r", title, _year,_month,_day, idMCU);
   
  printf("\033[0m\a\n");    //Normal+bell               
}

#endif