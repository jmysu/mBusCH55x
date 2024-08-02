/*
 *  cdcCMDs.c
 *
 *    CH55x CDC CMDs
 * 
 *    by jimmy.su @ 2024
 * 
 *    
 *    
 *    LINKFLAGS: -mmcs51 --iram-size 256 --xram-size 256 --code-size 16384 --out-fmt-ihx --xram-size 768 --xram-loc 256      
 *
 *    Flash: [====      ]  41.9% (used 6869 bytes from 16384 bytes) //Use standard printf_large
 *    Flash: [===       ]  34.1% (used 5579 bytes from 16384 bytes) //Use xprintf
 * 
 *    Flash: [===       ]  32.5% (used 5325 bytes from 16384 bytes) //W/o TITLE_STAMP
 *    Flash: [====      ]  36.2% (used 5933 bytes from 16384 bytes) // w/ TITLE_STAMP
 * 
 *    Other memory:
 *    Name             Start    End      Size     Max     
 *  ---------------- -------- -------- -------- --------
 *    PAGED EXT. RAM                         0      256   
 *    EXTERNAL RAM     0x0100   0x014b      76      768   
 *    ROM/EPROM/FLASH  0x0000   0x15ca    5579    16384   
 * 
 *          __   __
 *    P3.2 -|o\_/ |- 3.3V (connect to GND via 0.1uF)
 *    P1.4 -|  C  |- 5.0V (connect to USB VBUS, connect to GND via 0.1uF)
 *    P1.5 -|  H  |- GND  (to USB GND)
 *    P1.6 -|  5  |- USB D-
 *    P1.7 -|  5  |- USB D+ (short 3.3v to BOOT)
 *    Reset-|  2  |- P3.4
 *    P3.1 -|  G  |- P3.3  
 *    P3.0 -|_____|- P1.1  
 * 
 */
// ===================================================================================
// Project:   ADC Transmitter Demo for CH551, CH552 and CH554
// Version:   v1.1
// Year:      2023
// Author:    Stefan Wagner
// Github:    https://github.com/wagiminator
// EasyEDA:   https://easyeda.com/wagiminator
// License:   http://creativecommons.org/licenses/by-sa/3.0/
// ===================================================================================
//
// Description:
// ------------
// Continuously transmits ADC sample values (pin P1.4) via USB-CDC.
//
// References:
// -----------
// - Blinkinlabs: https://github.com/Blinkinlabs/ch554_sdcc
// - Deqing Sun: https://github.com/DeqingSun/ch55xduino
// - Ralph Doncaster: https://github.com/nerdralph/ch554_sdcc
// - WCH Nanjing Qinheng Microelectronics: http://wch.cn
//
// Compilation Instructions:
// -------------------------
// - Chip:  CH551, CH552 or CH554
// - Clock: 16 MHz internal
// - Adjust the firmware parameters in src/config.h if necessary.
// - Make sure SDCC toolchain and Python3 with PyUSB is installed.
// - Press BOOT button on the board and keep it pressed while connecting it via USB
//   with your PC.
// - Run 'make flash' immediatly afterwards.
// - To compile the firmware using the Arduino IDE, follow the instructions in the 
//   .ino file.
//
// Operating Instructions:
// -----------------------
// - Connect the board via USB to your PC. It should be detected as a CDC device.
// - Open a serial monitor or a serial plotter and select the correct serial port
//   (BAUD rate doesn't matter).
// - The ADC sample values are continuously transmitted via CDC.
// - Use a variable resistor (5V - P1.4 - GND) to change values.

#undef F_CPU
#define F_CPU 24000000
#warning "F_CPU 24MHz"
// ===================================================================================
// Libraries, Definitions and Macros
// ===================================================================================
#include <stdio.h>                        // for printf
#include <stdbool.h>

// Libraries
#include "src/config.h"                   // user configurations
#include "src/gpio.h"                     // GPIO functions
#include "src/system.h"                   // system functions
#include "src/delay.h"                    // delay functions
#include "src/usb_cdc.h"                  // USB-CDC serial functions

#include "myCmds.h"
#include "ansiColor.h"
#include "xprintf.h"
 
#ifdef _TITLE_STAMP_
 #include "myDate.h"
#endif


// Prototypes for used interrupts
void USB_interrupt(void);
void USB_ISR(void) __interrupt(INT_NO_USB) {
  USB_interrupt();
}

// ===================================================================================
// printf
// ===================================================================================
//#define printf printf_tiny
//#define ALWAYS_PRINT_UNSIGNED

#define DBGCMD 1

int putchar(int c) {
  CDC_write(c & 0xFF);
  return c;
}
/*
int putchar (int ch)
{
  if(ch == '\n') putchar('\r');
  for(SBUF0 = ch; !TI0; );
  TI0 = 0;
  return ch;
}

int getchar (void)
{
  while(!RI0);
  RI0 = 0;
  return SBUF0;
}
*/
__xdata uint8_t buf[32];  //CDC input buffer
__xdata uint8_t buf1[32];  //CDC input buffer copy
__xdata uint8_t counterParam;
 
__code uint16_t __at(ROM_CHIP_ID_HX) chid_hx;
__code uint16_t __at(ROM_CHIP_ID_HI) chid_hi;
__code uint16_t __at(ROM_CHIP_ID_LO) chid_lo;
// ===================================================================================
// Main Function
// ===================================================================================
void main(void) {
char cPtr=0;
bool isInput=false;

  // Setup
  CLK_config();                           // configure system clock
  DLY_ms(10);                             // wait for clock to settle
  CDC_init();                             // init USB CDC
  DLY_ms(1000);                           // WAIT 1-second
 

  while (!CDC_ready()) {} //wait USBCDC ready to write1
  //DLY_ms(500);    
  //=========================================================================
  //TIMESTAMP & ID & TITLE  (3241-2337)bytes
    #ifdef _TITLE_STAMP_
      PRINTF("\033[H\033[J");                                                             // Clear screen
      PRINTF("\033[" ANSI_GREEN "m");                                                     // Green
      PRINTF(__TIMESTAMP__);                                                              // Build timestamp
      PRINTF("   CH5%2x:%02X-%04X-%04X\n", CHIP_ID, chid_hx & 0x00FF, chid_hi, chid_lo);  // Chip ID
      PRINTF("\033[" ANSI_YELLOW "m");                                                    // Yellow
      getMyDate();                                                                        // Get date in number from __DATE__
      PRINTF("\033[1m");                                                                  // BOLD
      PRINTF("___%s.%02u%02u%02u___\n", "cdcCMDs", _year,_month,_day);                    // Project Name
      PRINTF("\033[0m\a");                   
      CDC_flush();
    #endif                                               // Normal
    //=========================================================================
  PIN_output(P17);            

  cmdHelp();
  PRINTF("CMD>");
  CDC_flush(); 

  // Loop
  while(1) {

      while (CDC_available()) { //wait input
          isInput = true;
          PIN_high(P17);

          char c = CDC_read();
          buf[cPtr++]=c;
          CDC_writeflush(c);
          
          if ((c=='\n') && (cPtr>2)){ // Got NewLine
            buf[cPtr]='\0';           // Null terminated!
      
            #if (DBGCMD) 
              for (int i=0;i<cPtr;i++) PRINTF("|%02X", buf[i]);
              PRINTF("|--->[%d]%s", cPtr, buf);
              CDC_flush();
              cPtr-=2;
              buf[cPtr]='\0';

              PRINTF("cmd:%s\n", cmdParam(0));
              //printf("p1:%s ", cmdParam(1));
              //printf("p2:%s ", cmdParam(2));
            #endif
            char *cmd = cmdParam(0);
            if (cmd!=NULL) cmdMatch(cmd);
 
            PRINTF("CMD>");
            CDC_flush();
            cPtr=0;                   // reset buffer      
            isInput=false;     
            }
          }

      if (!isInput){
          PIN_toggle(P17);             
          DLY_ms(500);
          }
        
      }

}
