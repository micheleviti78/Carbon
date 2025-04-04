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

#include <carbon/mp_port/mpcarbon.h>

#include <py/builtin.h>
#include <py/compile.h>
#include <py/gc.h>
#include <py/lexer.h>
#include <py/mperrno.h>
#include <py/persistentcode.h>
#include <py/runtime.h>
#include <py/stackctrl.h>
#include <shared/runtime/gchelper.h>

#include <string.h>

#include <carbon/diag.hpp>

// Initialise the runtime.
#if MICROPY_PY_THREAD
void mp_carbon_init(void *stack, size_t stack_len, void *gc_heap,
                    size_t gc_heap_size, void *sp) {
    mp_thread_init(stack, stack_len);
    mp_stack_set_top(sp);
    gc_init(gc_heap, (uint8_t *)gc_heap + gc_heap_size);
    mp_init();
}
#else
void mp_carbon_init(void *gc_heap, size_t gc_heap_size, void *sp) {
    mp_thread_init(stack, stack_len);
    mp_stack_set_top(sp);
    gc_init(gc_heap, (uint8_t *)gc_heap + gc_heap_size);
    mp_init();
}
#endif

// Deinitialise MicroPython.
void mp_carbon_deinit() { mp_deinit(); }

#if MICROPY_ENABLE_COMPILER
// Compile and execute the given source script (Python text).
void mp_carbon_exec_str(const char *src) {
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
        DIAG(MP "Uncaught exception from source script");
        mp_obj_print_exception(&mp_plat_print, (mp_obj_t)nlr.ret_val);
    }
}
#endif

#if MICROPY_PERSISTENT_CODE_LOAD
void mp_carbon_exec_mpy(const uint8_t *mpy, size_t len) {
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
        DIAG(MP "Uncaught exception from precompiled code");
        mp_obj_print_exception(&mp_plat_print, (mp_obj_t)nlr.ret_val);
    }
}
#endif

// Called if an exception is raised outside all C exception-catching handlers.
void nlr_jump_fail(void *val) {
    DIAG(MP "micropython exception at %p", val);
    mp_obj_print_exception(&mp_plat_print, MP_OBJ_FROM_PTR(val));
    for (;;) {
    }
}

// There is no filesystem so opening a file raises an exception.
mp_lexer_t *mp_lexer_new_from_file(qstr filename) {
    mp_reader_t reader;
    mp_reader_new_file(&reader, filename);
    return mp_lexer_new(filename, reader);
}

void mp_reader_new_file(mp_reader_t *reader, qstr filename) {
    DIAG(MP "mp_reader_new_file");
}

mp_import_stat_t mp_import_stat(const char *path) {
    (void)path;
    DIAG(MP "mp_import_stat");
    return MP_IMPORT_STAT_NO_EXIST;
}

mp_obj_t mp_builtin_open(size_t n_args, const mp_obj_t *args,
                         mp_map_t *kwargs) {
    DIAG(MP "mp_builtin_open");
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(mp_builtin_open_obj, 1, mp_builtin_open);

#ifndef NDEBUG
// Used when debugging is enabled.
void __assert_func(const char *file, int line, const char *func,
                   const char *expr) {
    DIAG(MP "assert function file %s, line %d, func %s, expr %s", file, line,
         func, expr);
    for (;;) {
    }
}
#endif