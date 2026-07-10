#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "linenoise/linenoise.h"

#define ASYNC_CONSOLE_LINE_BUF_LEN 256

extern struct linenoiseState g_ls;
extern char g_ln_buf[ASYNC_CONSOLE_LINE_BUF_LEN];
extern SemaphoreHandle_t g_log_mutex;

void async_console_io_init(void);
