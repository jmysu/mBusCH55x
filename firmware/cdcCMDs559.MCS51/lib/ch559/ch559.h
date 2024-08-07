// Copyright 2021 Takashi Toyoshima <toyoshim@gmail.com>. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef __ch559_h__
#define __ch559_h__

#include <stdbool.h>
#include <stdint.h>

#define CHIP_ID SAFE_MOD
#define ROM_CHIP_ID_ADDR 0x20

__code uint16_t __at(ROM_CHIP_ID_ADDR+0) chid_d0;
__code uint16_t __at(ROM_CHIP_ID_ADDR+1) chid_d1;
__code uint16_t __at(ROM_CHIP_ID_ADDR+2) chid_d2;
__code uint16_t __at(ROM_CHIP_ID_ADDR+3) chid_d3;
__code uint16_t __at(ROM_CHIP_ID_ADDR+4) chid_d4;
__code uint16_t __at(ROM_CHIP_ID_ADDR+5) chid_d5;
__code uint16_t __at(ROM_CHIP_ID_ADDR+6) chid_d6;
__code uint16_t __at(ROM_CHIP_ID_ADDR+7) chid_d7;

extern void initialize(void);

// Arduino-like APIs

extern void delayMicroseconds(uint32_t us);
extern void delay(uint32_t ms);
extern uint32_t GetChipUniqueID(void); //ChipID559
extern void BOOT_prepare(void);
extern void RST_now(void);
extern void (*runBootloaderF400)(void);

#endif  // __ch559_h__
