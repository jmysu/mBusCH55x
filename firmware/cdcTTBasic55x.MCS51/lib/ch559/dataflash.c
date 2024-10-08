/*
 * dataflash.c
 *
 * ch559 data flash 0xf000~0xf3ff
 * 
 * 
 * 
 */
// Copyright 2021 Takashi Toyoshima <toyoshim@gmail.com>. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "dataflash.h"
#include "io.h"

static const uint16_t flash_size = 0x0400;
__code unsigned char* data_flash_F000 = (__code uint8_t*)0xf000;


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

//erase flash in 1K size
static bool erase(__code uint8_t* addr) {
  enter();
  ROM_ADDR_H = (uint16_t)addr >> 8;
  ROM_ADDR_L = (uint16_t)addr;
  ROM_CTRL = 0xa6;
  bool result = ROM_STATUS == 0xc0;
  leave();
  return result;
}


bool dataflash_read(uint8_t* data, uint16_t size) {
  for (uint16_t i = 0; i < size; ++i)
    data[i] = data_flash_F000[i];
  return true;
}

uint8_t dataflash_at(uint16_t addr) {
  return  data_flash_F000[addr];
}

bool dataflash_write(const uint8_t* data, uint16_t size) {
  erase(data_flash_F000); //1K erase
  enter();
  bool result = true;

  for (int16_t i = 0; i < size; i += 2) {
    ROM_ADDR_H = (uint16_t)(data_flash_F000 + i) >> 8;
    ROM_ADDR_L = (uint16_t)(data_flash_F000 + i);
    ROM_DATA_H = *(data+1+i);
    ROM_DATA_L = *(data+i);
    ROM_CTRL = 0x9a;
    if (ROM_STATUS != 0xc0) {
      result = false;
      break;
      }
    }

  leave();
  return result;
}
/*
//restore dataflash from codeflash
static bool restore(void) {
  if (!erase(data_flash_F000))
    return false;
  enter();
  bool result = true;
  uint16_t addr = (uint16_t)data_flash_F000;
  for (int16_t i = flash_size - 2; i >= 0; i -= 2) {
    ROM_ADDR_H = (addr + i) >> 8;
    ROM_ADDR_L = addr + i;
    ROM_DATA_H = code_flash[i + 1];
    ROM_DATA_L = code_flash[i + 0];
    ROM_CTRL = 0x9a;
    if (ROM_STATUS != 0xc0) {
      result = false;
      break;
    }
  }
  leave();
  return result;
}

// To protect data safely, the initialization code can restore data from a
// back-up if shutdown happend during the write operations.
// 1. Checks the MAGIC in the data flash and returns if it's valid.
// 2. Checks the MAGIC in the code flash, and restores it and returns if it's
//    valid.
// 3. Initializes the data flash with the MAGIC.
bool flash_init(uint32_t new_magic, bool force) {
  magic = new_magic;
  __code uint32_t* flash = (__code uint32_t*)data_flash_F000;
  if (magic == *flash) {
    return true;
  }
  flash = (__code uint32_t*)code_flash;
  if (magic == *flash) {
    // Maybe the previous shutdown happens during restore operation. Redo.
    return restore();
  }
  // No data. Let's initialize with the magic if `force` is set.
  if (!force || !erase(data_flash_F000)) {
    return false;
  }
  enter();
  bool result = true;
  ROM_ADDR_H = (uint16_t)data_flash_F000 >> 8;
  ROM_ADDR_L = (uint16_t)data_flash_F000;
  ROM_DATA_H = magic >> 8;
  ROM_DATA_L = magic;
  ROM_CTRL = 0x9a;
  if (ROM_STATUS != 0xc0)
    result = false;
  ROM_ADDR_H = (uint16_t)(data_flash_F000 + 2) >> 8;
  ROM_ADDR_L = (uint16_t)(data_flash_F000 + 2);
  ROM_DATA_H = magic >> 24;
  ROM_DATA_L = magic >> 16;
  ROM_CTRL = 0x9a;
  if (ROM_STATUS != 0xc0)
    result = false;
  leave();
  return result;
}

// To be safe, the write operations;
// 1. erases the code flash area.
// 2. stores new data into the code flash area. MAGIC is stored lastly.
// 3. erases the data flash area.
// 4. copies the new data in code flash to the data flash in reversed order so
//    to copy the MAGIC lastly.
// If operations are interrupted by a shutdown, we can recover safely.
// If it happens on 1 and 2, the data flash is still valid.
// If it does on 3 and 4, the MAGIC in the data flash is missed, and data is
// restored from the back-up in the code flash, preparated at the step 2.
bool flash_write(uint16_t offset, const uint8_t* data, uint16_t size) {
  // Protect magic region.
  if (offset < 4)
    return false;
  // Prepare block data in the code flash.
  erase(code_flash);
  enter();
  bool result = true;
  // Store the magic last. Thus, data can be consistent if the magic is there.
  for (int16_t i = flash_size - 2; i >= 0; i -= 2) {
    ROM_ADDR_H = (uint16_t)(code_flash + i) >> 8;
    ROM_ADDR_L = (uint16_t)(code_flash + i);
    ROM_DATA_H = (offset <= (i + 1) && (i + 1) < (offset + size))
                     ? data[i + 1 - offset]
                     : data_flash_F000[i + 1];
    ROM_DATA_L =
        (offset <= i && i < (offset + size)) ? data[i - offset] : data_flash_F000[i];
    ROM_CTRL = 0x9a;
    if (ROM_STATUS != 0xc0) {
      result = false;
      break;
    }
  }
  leave();
  // Now the backup in the code flash works. Let's copy it to the data flash.
  // Initialization code can restore data from the backup if this operation
  // does not complete due to unexpected shutdown.
  restore();
  return result;
}

bool flash_read(uint16_t offset, uint8_t* data, uint16_t size) {
  __code uint32_t* flash = (__code uint32_t*)data_flash;
  if (magic != *flash)
    return false;
  for (uint16_t i = 0; i < size; ++i)
    data[i] = data_flash_F000[offset + i];
  return true;
}

*/