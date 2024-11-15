/**
 ******************************************************************************
 * @file           io_utils.h
 * @author         Michele Viti <micheleviti78@gmail.com>
 * @date           Nov. 2024
 * @brief          io utilities
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

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <ff.h>

FRESULT copy_file(const char *srcPath, const char *dstPath);

#ifdef __cplusplus
}
#endif