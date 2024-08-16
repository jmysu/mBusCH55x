// ===================================================================================
// NeoPixel (Addressable LED) Functions for CH551, CH552 and CH554            * v1.1 *
// ===================================================================================
//
// Basic control functions for 800kHz addressable LEDs (NeoPixel). A simplified 
// protocol is used which should work with most LEDs.
//
// The following must be defined in config.h:
// PIN_NEO - pin connected to DATA-IN of the pixel strip (via a ~330 ohms resistor).
// NEO_GRB - type of pixel: NEO_GRB or NEO_RGB
// System clock frequency must be at least 6 MHz.
//
// Further information:     https://github.com/wagiminator/ATtiny13-NeoController
// 2023 by Stefan Wagner:   https://github.com/wagiminator

// ===================================================================================
// Libraries, Variables and Constants
// ===================================================================================
#include "neo.h"
 
//#define NEOPIN PIN_asm(PIN_NEO)     // convert PIN_NEO for inline assembly
uint8_t NEOdata[NEO_COUNT][3];

// ===================================================================================
// Protocol Delays
// ===================================================================================
// There are three essential conditions:
// - T0H (HIGH-time for "0"-bit) must be max.  500ns
// - T1H (HIGH-time for "1"-bit) must be min.  625ns
// - TCT (total clock time) must be      min. 1150ns
// The bit transmission loop takes 11 clock cycles.
//
// Tune for 12MHz w/ PLL 48MHz USB clock
//
#if F_CPU == 12000000     // 12MHz system clock, Fsys/USB CLOCK:48MHz, 1/48=20.8ns

  #define DELAY220ns \
    nop             \
    nop             \
    nop             \
    nop             \
    nop             \
    nop             \
    nop             \
    nop             \
    nop             \
    nop             \
    nop             // 11x0.02 = 220ns              
  //
   #define DELAY100ns \
    nop             \
    nop             \
    nop             \
    nop             \
    nop             // 5x0.02 = 100ns
  //
#else
  #error Unsupported system clock frequency for NeoPixels!
#endif

// ===================================================================================
// Send a Data Byte to the Pixels String
// ===================================================================================
// This is the most time sensitive part. Outside of the function, it must be 
// ensured that interrupts are disabled and that the time between the 
// transmission of the individual bytes is less than the pixel's latch time.
void NEO_sendByte(uint8_t data) { 
  data;                 // stop unreferenced argument warning
  __asm
    .even
    mov  r7, #8         ; 2 CLK - 8 bits to transfer
    xch  a, dpl         ; 2 CLK - data byte -> accu

  01$:
    rlc  a              ; 1 CLK - data bit -> carry (MSB first)
    setb NEOPIN         ; 2 CLK - NEO pin HIGH HHHHHHHHHHHHHHHHH
    DELAY220ns          ; 220 ns
    DELAY100ns          ; 100 ns

    mov  NEOPIN, c      ; 2 CLK - "0"-bit? -> NEO pin LOW now
    //T1H_DELAY         ; x CLK - ------------------------------
    DELAY220ns          ; 220 ns
    DELAY100ns          ; 100 ns

    clr  NEOPIN         ; 2 CLK - "1"-bit? -> NEO pin LOW a little later
    //TCL_DELAY         ; y CLK - TCL delay LLLLLLLLLLLLLLLLLLLL
    DELAY220ns          ; 220 ns
    DELAY100ns          ; 100 ns

    djnz r7, 01$        ; 2/4|5|6 CLK - repeat for all bits
  __endasm;
}

// ===================================================================================
// Write Color to a Single Pixel
// ===================================================================================
void NEO_writeColor(uint8_t r, uint8_t g, uint8_t b) {
  #if defined (NEO_GRB)
    EA=0; NEO_sendByte(g); NEO_sendByte(r); NEO_sendByte(b); EA=1;
  #elif defined (NEO_RGB)
    EA=0; NEO_sendByte(r); NEO_sendByte(g); NEO_sendByte(b); EA=1;
  #else
    #error Wrong or missing NeoPixel type definition!
  #endif
}

// ===================================================================================
// Write Hue Value (0..191) and Brightness (0..2) to a Single Pixel
// ===================================================================================
void NEO_writeHue(uint8_t hue, uint8_t bright) {
  uint8_t phase = hue >> 6;
  uint8_t step  = (hue & 63) << bright;
  uint8_t nstep = (63 << bright) - step;
  switch(phase) {
    case 0:   NEO_writeColor(nstep,  step,     0); break;
    case 1:   NEO_writeColor(    0, nstep,  step); break;
    case 2:   NEO_writeColor( step,     0, nstep); break;
    default:  break;
  }
}