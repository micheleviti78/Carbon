/**
 ******************************************************************************
 * @file           carbon_mp_init.cpp
 * @author         Michele Viti <micheleviti78@gmail.com>
 * @date           Jan. 2024
 * @brief          micropython init
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

// This array is the MicroPython GC heap.
static char heap[8 * 1024];

// Initialise the runtime.
inline void mp_embed_init(void *gc_heap, size_t gc_heap_size) {
    mp_stack_ctrl_init();
    gc_init(gc_heap, (uint8_t *)gc_heap + gc_heap_size);
    mp_init();
}
