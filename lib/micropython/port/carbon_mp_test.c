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

#include "carbon_mp_test.h"
#include "carbon_mp.h"

static const char *example_1 =
    "print('hello world!', list(x + 1 for x in range(10)), end='eol\\n')";

static const char *example_2 = "for i in range(10):\n"
                               "    print('iter {:08}'.format(i))\n"
                               "\n"

                               "import gc\n"
                               "print('run GC collect')\n"
                               "gc.collect()\n"
                               "\n"
                               "print('finish')\n";

static const char *example_3 = "def addition(number1, number2):\n"
                               "result = number1 + number2\n"
                               "print(' Addition result : ',result)\n"
                               "def area(radius):\n"
                               "result = 3.14 * radius * radius\n"
                               "return result\n"
                               "addition(5, 3)\n"
                               "print(' Area of the circle is ',area(2))\n"
                               "import gc\n"
                               "print('run GC collect')\n"
                               "gc.collect()\n"
                               "\n"
                               "print('finish')\n";

void carbon_mp_test() {
    mp_embed_exec_str(example_1);
    mp_embed_exec_str(example_2);
    mp_embed_exec_str(example_3);
}
