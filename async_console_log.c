#include "async_console_log.h"

#include <stdarg.h>
#include <stdio.h>

#include "async_console_io.h"
#include "esp_log.h"
#include "linenoise/linenoise.h"

static int async_console_log_vprintf(const char *fmt, va_list args) {
    static char s_log_buf[1024];
    static bool s_line_in_progress = false;

    xSemaphoreTake(g_log_mutex, portMAX_DELAY);

    int len = vsnprintf(s_log_buf, sizeof(s_log_buf), fmt, args);
    if (len < 0) {
        xSemaphoreGive(g_log_mutex);
        return len;
    }

    int write_len = (len >= sizeof(s_log_buf)) ? (sizeof(s_log_buf) - 1) : len;
    bool ends_with_newline = (write_len > 0 && s_log_buf[write_len - 1] == '\n');

    if (!s_line_in_progress) {
        linenoiseHide(&g_ls);
    }

    fwrite(s_log_buf, 1, write_len, stdout);

    if (ends_with_newline) {
        linenoiseShow(&g_ls);
        s_line_in_progress = false;
    } else {
        s_line_in_progress = true;
    }

    xSemaphoreGive(g_log_mutex);
    return len;
}

void async_console_log_hook_install(void) {
    esp_log_set_vprintf(async_console_log_vprintf);
}
