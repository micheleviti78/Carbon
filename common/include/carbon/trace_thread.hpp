/**
 ******************************************************************************
 * @file           trace_thread.hpp
 * @author         Michele Viti <micheleviti78@gmail.com>
 * @date           Jan. 2024
 * @brief          starting trace thread
 ******************************************************************************
 * @attention
 * Copyright (c) 2024 Michele Viti.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

#pragma once

#ifdef FREERTOS_USE_TRACE

#ifdef __cplusplus
extern "C" {
#endif

void start_trace_thread(void);

#ifdef __cplusplus
}
#endif

#endif
