/**
 ******************************************************************************
 * @file           carbon_mp.c
 * @author         Michele Viti <micheleviti78@gmail.com>
 * @date           Mar. 2024
 * @brief          carbon micropython API
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

#include "carbon_mp.h"

#include "py/builtin.h"
#include "py/compile.h"
#include "py/gc.h"
#include "py/lexer.h"
#include "py/mperrno.h"
#include "py/persistentcode.h"
#include "py/runtime.h"
#include "py/stackctrl.h"
#include "shared/runtime/gchelper.h"

#include <string.h>

#include <carbon/diag.hpp>
#include <printf.h>

// Initialise the runtime.
void mp_embed_init(void *gc_heap, size_t gc_heap_size, void *sp) {
#if MICROPY_PY_THREAD
    mp_thread_init();
#endif
    mp_stack_set_top(sp);
    gc_init(gc_heap, (uint8_t *)gc_heap + gc_heap_size);
    mp_init();
}

// Deinitialise MicroPython.
void mp_embed_deinit() { mp_deinit(); }

#if MICROPY_ENABLE_COMPILER
// Compile and execute the given source script (Python text).
void mp_embed_exec_str(const char *src) {
    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        // Compile, parse and execute the given string.
        mp_lexer_t *lex = mp_lexer_new_from_str_len(MP_QSTR__lt_stdin_gt_, src,
                                                    strlen(src), 0);
        qstr source_name = lex->source_name;
        mp_parse_tree_t parse_tree = mp_parse(lex, MP_PARSE_FILE_INPUT);
        mp_obj_t module_fun = mp_compile(&parse_tree, source_name, true);
        mp_call_function_0(module_fun);
        nlr_pop();
    } else {
        // Uncaught exception: print it out.
        mp_obj_print_exception(&mp_plat_print, (mp_obj_t)nlr.ret_val);
    }
}
#endif

#if MICROPY_PERSISTENT_CODE_LOAD
void mp_embed_exec_mpy(const uint8_t *mpy, size_t len) {
    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        // Execute the given .mpy data.
        mp_module_context_t *ctx = m_new_obj(mp_module_context_t);
        ctx->module.globals = mp_globals_get();
        mp_compiled_module_t cm;
        cm.context = ctx;
        mp_raw_code_load_mem(mpy, len, &cm);
        mp_obj_t f = mp_make_function_from_raw_code(cm.rc, ctx, MP_OBJ_NULL);
        mp_call_function_0(f);
        nlr_pop();
    } else {
        // Uncaught exception: print it out.
        mp_obj_print_exception(&mp_plat_print, (mp_obj_t)nlr.ret_val);
    }
}
#endif

// Called if an exception is raised outside all C exception-catching handlers.
void nlr_jump_fail(void *val) {
    printf_("\r\nmicropython exception at %p", val);
    for (;;) {
    }
}

// There is no filesystem so opening a file raises an exception.
mp_lexer_t *mp_lexer_new_from_file(qstr filename) {
    mp_raise_OSError(MP_ENOENT);
}

mp_import_stat_t mp_import_stat(const char *path) {
    (void)path;
    return MP_IMPORT_STAT_NO_EXIST;
}

#ifndef NDEBUG
// Used when debugging is enabled.
void __assert_func(const char *file, int line, const char *func,
                   const char *expr) {
    RAW_DIAG("assert function file %s, line %d, func %s, expr %s", file, line,
             func, expr);
    for (;;) {
    }
}
#endif