/**
 ******************************************************************************
 * @file           led.c
 * @author         Michele Viti <micheleviti78@gmail.com>
 * @date           Mar. 2024
 * @brief          micropython LED module
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

#include "py/runtime.h"

void toggle_orange();
void toggle_red();

static mp_obj_t led_toggle_orange(void) {
    toggle_orange();
    return mp_const_none;
}

static mp_obj_t led_toggle_red(void) {
    toggle_red();
    return mp_const_none;
}

static MP_DEFINE_CONST_FUN_OBJ_0(led_toggle_orange_obj, led_toggle_orange);

static MP_DEFINE_CONST_FUN_OBJ_0(led_toggle_red_obj, led_toggle_red);

static const mp_rom_map_elem_t led_module_globals_table[] = {
    {MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_led)},
    {MP_ROM_QSTR(MP_QSTR_toggle_orange), MP_ROM_PTR(&led_toggle_orange_obj)},
    {MP_ROM_QSTR(MP_QSTR_toggle_red), MP_ROM_PTR(&led_toggle_red_obj)}};

static MP_DEFINE_CONST_DICT(led_module_globals, led_module_globals_table);

const mp_obj_module_t led_module = {
    .base = {&mp_type_module},
    .globals = (mp_obj_dict_t *)&led_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_led, led_module);