/**
 ******************************************************************************
 * @file           mp_thread.c
 * @author         Michele Viti <micheleviti78@gmail.com>
 * @date           Mar. 2024
 * @brief          source file for the micropython task
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

#include <carbon/diag.hpp>

#include <lwip/api.h>

#include <carbon/mp_port/mpcarbon.h>

#include <printf.h>

#include <sd_diskio.h>

#include <shared/runtime/pyexec.h>

#include <cmsis_os.h>
#include <task.h>

#include <stdint.h>

struct netconn *connection = NULL;
struct netconn *new_connection = NULL;

#define CARBON_MP_CONSOLE_PORT 6666

#define MICROPY_TASK_PRIORITY (1)
#define MICROPY_TASK_STACK_SIZE 2097152U /*2 MB size micropython stack*/
#define MICROPY_TASK_STACK_LEN (MICROPY_TASK_STACK_SIZE / sizeof(StackType_t))
#define MICROPYTHON_HEAP_SIZE 2097152U /*2 MB size micropython heap*/

// This is the static memory (TCB and stack) for the main MicroPython task
static StaticTask_t mpTaskTCB
    __attribute__((aligned(8), section(".sdram_bank2")));

static StackType_t mpTaskStack[MICROPY_TASK_STACK_LEN]
    __attribute__((aligned(8), section(".sdram_bank2")));

static uint8_t micropython_heap[MICROPYTHON_HEAP_SIZE]
    __attribute__((aligned(32), section(".sdram_bank2")));

uintptr_t cortex_m7_get_sp(void);

static uint8_t *sp;

static void TASK_MicroPython(void *pvParameters);

static bool setup_connection();

static void close_connection();

void TASK_MicroPython(void *pvParameters) {
    sp = (uint8_t *)cortex_m7_get_sp();

    DIAG(MP "starting micropython");
    DIAG(MP "stack at %p size %u", mpTaskStack, MICROPY_TASK_STACK_SIZE);
    DIAG(MP "heap at %p size %u", micropython_heap, MICROPYTHON_HEAP_SIZE);

    mp_carbon_init(&mpTaskStack[0], MICROPY_TASK_STACK_LEN,
                   &micropython_heap[0], MICROPYTHON_HEAP_SIZE, sp);

    if (pvParameters) {
        DIAG(MP "executing script at %p", pvParameters);
        mp_carbon_exec_str((const char *)pvParameters);
        DIAG(MP "python script execution terminated");
        vPortFree(pvParameters);
    } else {
        DIAG(MP "no python script");
    }

    if (!setup_connection()) {
        DIAG(MP "cannot setup connection");
        for (;;) {
            osDelay(10000);
        }
    }

    int err = netconn_accept(connection, &new_connection);

    if (err != ERR_OK) {
        DIAG(MP "error while connecting %d", err);
        for (;;) {
            osDelay(10000);
        }
    }

    // main script is finished, so now go into REPL mode.
    // the REPL mode can change, or it can request a soft reset.
    for (;;) {
        int ret = pyexec_friendly_repl();
        if (ret != 0) {
            DIAG(MP "error in console %d", ret);
            close_connection();
            DIAG(MP "waiting for new connection");
            err = netconn_accept(connection, &new_connection);
            if (err != ERR_OK) {
                DIAG(MP "error while reconnection %d", err);
                break;
            }
        } else if (!new_connection) {
            DIAG(MP "waiting for new connection");
            err = netconn_accept(connection, &new_connection);
            if (err != ERR_OK) {
                DIAG(MP "error while reconnection %d", err);
                break;
            }
        }
    }

    DIAG(MP "not recoverable error");

    while (1)
        osDelay(10000);
}

void start_micropython() {
    FIL fs_read;
    UINT byte_read;
    uint8_t *readBuffer = NULL;

    FRESULT fres = f_open(&fs_read, "0:/main.py", FA_READ);

    if (fres != 0) {
        if (fres == FR_NOT_ENABLED)
            DIAG(MP "SD card not present or not mounted");
        else if (fres == FR_NO_FILE)
            DIAG(MP "file main.py not found");
        else
            DIAG(MP "error opening file %d", fres);
    } else {

        FILINFO file_info;

        fres = f_stat("0:/main.py", &file_info);

        if (fres != 0) {
            DIAG(MP "error reading file stats %d", fres);
        } else {
            DIAG(MP "file size %lu", file_info.fsize);
        }

        readBuffer = (uint8_t *)pvPortMalloc(file_info.fsize + 1);

        if (readBuffer) {
            size_t byte_to_read = file_info.fsize;
            uint8_t *head = readBuffer;
            while (byte_to_read > 0) {
                fres = f_read(&fs_read, head, byte_to_read, &byte_read);
                if (fres != 0) {
                    DIAG(MP "error copying file %d", fres);
                    vPortFree(readBuffer);
                    readBuffer = NULL;
                    break;
                }
                byte_to_read -= byte_read;
                head += byte_read;
            }
            *(readBuffer + file_info.fsize) = 0;
        }

        /* Close open files */
        f_close(&fs_read);
        DIAG(MP "file closed");
    }

    TaskHandle_t taskHandle = xTaskCreateStatic(
        TASK_MicroPython, "MicroPy", MICROPY_TASK_STACK_LEN, readBuffer,
        MICROPY_TASK_PRIORITY, mpTaskStack, &mpTaskTCB);
    if (taskHandle == NULL) {
        DIAG(MP "failed to start the micropython task");
    }
}

bool setup_connection() {
    connection = netconn_new(NETCONN_TCP);
    if (!connection) {
        DIAG(MP "Failed to create new netconn");
        return false;
    }

    uint16_t port = CARBON_MP_CONSOLE_PORT;

    int err = netconn_bind(connection, nullptr, port);
    if (err != ERR_OK) {
        DIAG(MP "Can't bind to port %" PRIu16 ": %d", port, err);
        netconn_delete(connection);
        return false;
    }

    err = netconn_listen_with_backlog(connection, 1);
    if (err != ERR_OK) {
        DIAG(MP "Can't listen to port %" PRIu16 ": %d", port, err);
        netconn_delete(connection);
        return false;
    }

    DIAG(MP "Accepting connections on port %" PRIu16, port);

    return true;
}

mp_uint_t mp_carbon_stdout(const char *str, size_t len) {
    if (new_connection) {
        int res = netconn_write(new_connection, str, len, NETCONN_COPY);
        if (res != ERR_OK) {
            close_connection();
        }
    } else {
        printf_("%.*s", (int)len, str);
    }
    return len;
}

static struct netbuf *carbon_mp_console_recv_netbuf = NULL;
static void *carbon_mp_console_recv_buf = NULL;
static uint16_t carbon_mp_console_recv_buf_size = 0;

int mp_carbon_stdint() {
    int err;
    if (new_connection) {
        if (carbon_mp_console_recv_buf_size == 0) {
            if (carbon_mp_console_recv_netbuf) {
                netbuf_delete(carbon_mp_console_recv_netbuf);
                carbon_mp_console_recv_netbuf = NULL;
            }
            err = netconn_recv(new_connection, &carbon_mp_console_recv_netbuf);
            if (err == ERR_OK && carbon_mp_console_recv_netbuf) {
                netbuf_data(carbon_mp_console_recv_netbuf,
                            &carbon_mp_console_recv_buf,
                            &carbon_mp_console_recv_buf_size);
                char c = *((char *)carbon_mp_console_recv_buf);
                carbon_mp_console_recv_buf =
                    (char *)carbon_mp_console_recv_buf + 1;
                carbon_mp_console_recv_buf_size--;
                return c;
            } else {
                DIAG(MP "error while receiving data %d", err);
                close_connection();
                return 0;
            }
        } else {
            char c = *((char *)carbon_mp_console_recv_buf);
            carbon_mp_console_recv_buf = (char *)carbon_mp_console_recv_buf + 1;
            carbon_mp_console_recv_buf_size--;
            return c;
        }
    }
    return 0;
}

void close_connection() {
    if (carbon_mp_console_recv_netbuf) {
        netbuf_delete(carbon_mp_console_recv_netbuf);
        carbon_mp_console_recv_netbuf = NULL;
    }
    carbon_mp_console_recv_buf_size = 0;
    if (new_connection) {
        netconn_close(new_connection);
        netconn_delete(new_connection);
        new_connection = NULL;
    }
    DIAG(MP "client disconnected");
}