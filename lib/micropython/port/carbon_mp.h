/**
 ******************************************************************************
 * @file           carbon_mp.h
 * @author         Michele Viti <micheleviti78@gmail.com>
 * @date           Mar. 2024
 * @brief          carbon micropython API
 ******************************************************************************
 * @attention
 * Copyright (c) 2022 Michele Viti.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

#ifndef MICROPY_INCLUDED_CARBON_MP_H
#define MICROPY_INCLUDED_CARBON_MP_H

#include <stddef.h>
#include <stdint.h>

#include "mpconfigport.h"

#ifdef __cplusplus
extern "C" {
#endif

void mp_embed_init(void *gc_heap, size_t gc_heap_size);
void mp_embed_deinit();

#if MICROPY_ENABLE_COMPILER
void mp_embed_exec_str(const char *src);
#endif

#if MICROPY_PERSISTENT_CODE_LOAD
void mp_embed_exec_mpy(const uint8_t *mpy, size_t len);
#endif

#ifdef __cplusplus
}
#endif

#endif // MICROPY_INCLUDED_CARBON_MP_H