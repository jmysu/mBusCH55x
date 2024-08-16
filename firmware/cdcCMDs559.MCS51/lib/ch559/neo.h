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

#pragma once
#include <stdint.h>
#include "io.h"
#include "gpio.h"


#define NEO_GRB
#define NEO_COUNT 8
extern uint8_t NEOdata[NEO_COUNT][3]; //count x RGB
// SDCC will add _ for global naming magling!!!
#define NEOPIN _P1_7


void NEO_sendByte(uint8_t data);                          // send a single byte to the pixels
void NEO_writeColor(uint8_t r, uint8_t g, uint8_t b);     // write color to a single pixel
void NEO_writeHue(uint8_t hue, uint8_t bright);           // hue (0..191), brightness (0..2)