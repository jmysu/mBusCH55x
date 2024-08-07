#ifndef TTBASIC_H
#define TTBASIC_H

#include <stdlib.h> 	// rnd
#include <stdint.h>

#include "led.h"		// led
#include "flash.h"  	// save, load, boot
#include "serial.h"  // debug

// TinyBASIC symbols
#define SIZE_LINE 80 //1行入力文字数と終端記号のサイズ
#define SIZE_IBUF 80//中間コードバッファのサイズ

//#define SIZE_LIST 127//リスト保存領域のサイズ	 CH554/2 
//#warning "DataFlash 127"
#define SIZE_LIST 1000//リスト保存領域のサイズ CH559
#define BOOTARG_ADDR SIZE_LIST-1
#define _BOOTMARK_FE 0xFE //FIX bootmark to 0xFE 
#warning "DataFlash 1000"

#define SIZE_ARAY 32//配列領域のサイズ
#define SIZE_GSTK 6//GOSUB用スタックサイズ(2単位)
#define SIZE_LSTK 15//FOR用スタックサイズ(5単位)

// Depending on device functions
// TO-DO Rewrite these functions to fit your machine
#ifdef _ENABLE_CDC_
 #define STR_EDITION "cdc559-1000"
#else
 #define STR_EDITION "uart559-1000"
#endif

extern void basic(void);
extern uint8_t cdcBuffer[];  // cdc buffer
#endif