/**
 ******************************************************************************
 * @file           mp_mod_led.cpp
 * @author         Michele Viti <micheleviti78@gmail.com>
 * @date           Mar. 2024
 * @brief          micropython LED module implementation
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

#include <carbon/pin.hpp>

extern "C" {

void toggle_orange() { BSP_LED_Toggle(LED_ORANGE); }

void toggle_red() { BSP_LED_Toggle(LED_RED); }
}