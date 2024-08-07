/*
 * dataflash.c
 *
 * ch559 data flash 0xf000~0xf3ff
 *
 * ch559 config @0xFFFE
 *
 */
// Copyright 2021 Takashi Toyoshima <toyoshim@gmail.com>. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "dataflash.h"

#include "io.h"

static const uint16_t flash_size = 0x0400;
__code uint8_t* data_flash_F000 = (__code uint8_t*)0xF000;

static inline void enter_safe_mode(void) {
    SAFE_MOD = 0x55;
    SAFE_MOD = 0xaa;
}

static inline void leave_safe_mode(void) {
    SAFE_MOD = 0;
}

static void enter(void) {
    enter_safe_mode();
    GLOBAL_CFG |= bDATA_WE | bCODE_WE;
    leave_safe_mode();
}

static void leave(void) {
    enter_safe_mode();
    GLOBAL_CFG &= ~(bDATA_WE | bCODE_WE);
    leave_safe_mode();
}

// erase flash in 1K size
static bool erase(__code uint8_t* addr) {
    enter();
    ROM_ADDR_H = (uint16_t)addr >> 8;
    ROM_ADDR_L = (uint16_t)addr;
    ROM_CTRL = 0xa6;
    bool result = ROM_STATUS == 0xc0;
    leave();
    return result;
}

// read dataflash from 0xF000 into data[]
bool dataflash_read(uint8_t* data, uint16_t size) {
    for (uint16_t i = 0; i < size; ++i)
        data[i] = data_flash_F000[i];
    return true;
}

// For reading flash  (CH559)
uint8_t dataflash_at(uint16_t addr) {
    uint8_t c = data_flash_F000[addr];
    return c;
}

// write data[] into flash from 0xF000
bool dataflash_write(const uint8_t* data, uint16_t size) {
    erase(data_flash_F000);  // 1K erase
    enter();
    bool result = true;

    for (int16_t i = 0; i < size; i += 2) {
        ROM_ADDR_H = (uint16_t)(data_flash_F000 + i) >> 8;
        ROM_ADDR_L = (uint16_t)(data_flash_F000 + i);
        ROM_DATA_H = *(data + 1 + i);
        ROM_DATA_L = *(data + i);
        ROM_CTRL = 0x9a;
        if (ROM_STATUS != 0xc0) {
            result = false;
            break;
        }
    }

    leave();
    return result;
}
