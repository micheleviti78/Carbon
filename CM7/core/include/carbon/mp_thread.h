/**
 ******************************************************************************
 * @file           mp_thread.h
 * @author         Michele Viti <micheleviti78@gmail.com>
 * @date           Jan. 2024
 * @brief          header file for the micropython task
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

#ifndef MICROPY_INCLUDED_MPTASK_H
#define MICROPY_INCLUDED_MPTASK_H

#ifdef __cplusplus
extern "C" {
#endif

void start_micropython();

#ifdef __cplusplus
}
#endif

#endif // MICROPY_INCLUDED_CC3200_MPTASK_H
