/**
 ******************************************************************************
 * @file           mphalport.c
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

#include "py/mphal.h"
#include "py/obj.h"

#include <printf.h>

#include <cmsis_os.h>

#include <carbon/systime.hpp>

mp_uint_t mp_carbon_stdout(const char *str, size_t len);
int mp_carbon_stdint();

// Send string of given length to stdout, converting \n to \r\n.
mp_uint_t mp_hal_stdout_tx_strn(const char *str, size_t len) {
    return mp_carbon_stdout(str, len);
}

inline void mp_hal_set_interrupt_char(int c) {
    // printf_("\r\nmp_hal_set_interrupt_char, char:  %d", c);
}

int mp_hal_stdin_rx_chr(void) { return mp_carbon_stdint(); }

uintptr_t mp_hal_stdio_poll(uintptr_t poll_flags) {
    printf_("\r\nmp_hal_stdio_poll");
    return 0;
}

void mp_hal_delay_ms(mp_uint_t ms) {
    osDelay(ms);
}

void mp_hal_delay_us(mp_uint_t us) {
    printf_("\r\nmp_hal_delay_us");
    delayUs(us);
}

mp_uint_t mp_hal_ticks_ms(void) {
    printf_("\r\nmp_hal_ticks_ms");
    return 0;
}

mp_uint_t mp_hal_ticks_us(void) {
    printf_("\r\nmp_hal_ticks_us");
    return 0;
}

mp_uint_t mp_hal_ticks_cpu(void) {
    printf_("\r\nmp_hal_ticks_cpu");
    return 0;
}

mp_obj_t mp_builtin_open(size_t n_args, const mp_obj_t *args,
                         mp_map_t *kwargs) {
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(mp_builtin_open_obj, 1, mp_builtin_open);