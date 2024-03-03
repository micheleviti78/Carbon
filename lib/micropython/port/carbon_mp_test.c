/**
 ******************************************************************************
 * @file           carbon_mp_test.c
 * @author         Michele Viti <micheleviti78@gmail.com>
 * @date           Jan. 2024
 * @brief          micropython test
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
/* This file is part of the MicroPython project, http://micropython.org/
 * The MIT License (MIT)
 * Copyright (c) 2022-2023 Damien P. George
 */

#include "carbon_mp_test.h"
#include "carbon_mp.h"

// This is example 1 script, which will be compiled and executed.
static const char *example_1 =
    "print('hello world!', list(x + 1 for x in range(10)), end='eol\\n')";

// This is example 2 script, which will be compiled and executed.
static const char *example_2 = "for i in range(10):\n"
                               "    print('iter {:08}'.format(i))\n"
                               "\n"

                               "import gc\n"
                               "print('run GC collect')\n"
                               "gc.collect()\n"
                               "\n"
                               "print('finish')\n";

// This array is the MicroPython GC heap.
// static char heap[8 * 1024];

void carbon_mp_test() {
    // Run the example scripts (they will be compiled first).
    mp_embed_exec_str(example_1);
    mp_embed_exec_str(example_2);
}

