/**
 ******************************************************************************
 * @file           mpconfigport.h
 * @author         Michele Viti <micheleviti78@gmail.com>
 * @date           Jan. 2024
 * @brief          micropython config file
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

#include <alloca.h>
#include <stdint.h>

// options to control how MicroPython is built

// Use the minimal starting configuration (disables all optional features).
#define MICROPY_CONFIG_ROM_LEVEL (MICROPY_CONFIG_ROM_LEVEL_BASIC_FEATURES)

// MicroPython configuration.
#define MICROPY_ENABLE_COMPILER (1)
#define MICROPY_ENABLE_GC (1)

#define MICROPY_PY_GC (1)
#define MICROPY_PY_THREAD (1)
#define MICROPY_PY_THREAD_GIL (1)

#define MICROPY_LONGINT_IMPL (MICROPY_LONGINT_IMPL_MPZ)
#define MICROPY_FLOAT_IMPL (MICROPY_FLOAT_IMPL_FLOAT)
#define MICROPY_PY_CMATH (1)

#define MICROPY_HELPER_REPL (1)
#define MICROPY_PY_SYS_PS1_PS2 (1)

#define MICROPY_PY_RE (1)

#define MICROPY_BANNER_MACHINE "Carbon"

#define MP_STATE_PORT MP_STATE_VM

#define MICROPY_MPHALPORT_H "port/mphalport.h"

typedef intptr_t mp_int_t;   // must be pointer size
typedef uintptr_t mp_uint_t; // must be pointer size
typedef long mp_off_t;

// #define STATIC static
