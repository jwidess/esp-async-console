#include "async_console_io.h"

struct linenoiseState g_ls = {0};
char g_ln_buf[ASYNC_CONSOLE_LINE_BUF_LEN] = {0};
SemaphoreHandle_t g_log_mutex = NULL;

void async_console_io_init(void) {
    g_log_mutex = xSemaphoreCreateMutex();
    configASSERT(g_log_mutex != NULL);
}
