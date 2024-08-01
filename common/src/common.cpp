/**
 ******************************************************************************
 * @file           common.cpp
 * @author         Michele Viti <micheleviti78@gmail.com>
 * @date           Jul. 2024
 * @brief          DISCO-STM32H747 common macros
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

#include <carbon/common.hpp>
#include <carbon/irq.hpp>

extern "C" {
void carbon_assert(unsigned long line, const char *filename,
                   const char *message) {

    CARBON::IRQ::lockRecursive();
    RAW_DIAG("Assertion \"%s\" failed at line %lu in %s\n", message, line,
             filename);
    __asm volatile("BKPT #0\n");
    while (true) {
    }
}
}