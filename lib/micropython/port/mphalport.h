/**
 ******************************************************************************
 * @file           mphalport.h
 * @author         Michele Viti <micheleviti78@gmail.com>
 * @date           Mar. 2024
 * @brief          port for HAL function
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
#ifndef MICROPY_INCLUDED_MPHALPORT_H
#define MICROPY_INCLUDED_MPHALPORT_H

#include "py/obj.h"

void mp_hal_set_interrupt_char(int c);

mp_uint_t mp_hal_stdout_tx_strn(const char *str, size_t len);

int mp_hal_stdin_rx_chr(void);

uintptr_t mp_hal_stdio_poll(uintptr_t poll_flags);

void mp_hal_delay_ms(mp_uint_t ms);

void mp_hal_delay_us(mp_uint_t us);

mp_uint_t mp_hal_ticks_ms(void);

mp_uint_t mp_hal_ticks_us(void);

mp_uint_t mp_hal_ticks_cpu(void);

#endif