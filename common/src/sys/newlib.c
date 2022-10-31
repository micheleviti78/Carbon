/**
 ******************************************************************************
 * @file           newlib.c
 * @author         Michele Viti <micheleviti78@gmail.com>
 * @date           Oct. 2022
 * @brief          newlib lock
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

#include <reent.h>

#include <diag.hpp>

// NOLINTNEXTLINE
__extension__ typedef int __guard __attribute__((mode(__DI__)));

extern int FreeRTOS_errno;

void not_supported(void) {
    void *caller __attribute__((unused)) = __builtin_return_address(0);
    RAW_DIAG("not suported, call at %p", caller);
    while (1) {
    }
}

// extern void not_supported(void);

void __env_lock(void) { not_supported(); };

void __env_unlock(void) { not_supported(); };

void __malloc_lock(struct _reent *r) { not_supported(); }

void __malloc_unlock(struct _reent *r) { not_supported(); }

int *__errno() { return &FreeRTOS_errno; }

int *__wrap___errno() { return &FreeRTOS_errno; }

// NOLINTNEXTLINE
void __cxa_pure_virtual() { not_supported(); }

extern void not_supported();

// NOLINTNEXTLINE
int __cxa_guard_acquire(__guard *g) {
    not_supported();
    // NOLINTNEXTLINE
    return !*(char *)(g);
}

// NOLINTNEXTLINE
void __cxa_guard_release(__guard *g) {
    not_supported();
    // NOLINTNEXTLINE
    *(char *)g = 1;
}

// NOLINTNEXTLINE
void __cxa_guard_abort(__guard *g) { not_supported(); }