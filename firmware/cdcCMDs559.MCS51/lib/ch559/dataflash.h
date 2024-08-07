// Copyright 2021 Takashi Toyoshima <toyoshim@gmail.com>. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef __flash_h__
#define __flash_h__
 
#include <stdbool.h>
#include <stdint.h>


bool dataflash_write(const uint8_t* data, uint16_t size);
bool dataflash_read(uint8_t* data, uint16_t size);
unsigned char dataflash_at(uint16_t addr);

#endif  // __flash_h__