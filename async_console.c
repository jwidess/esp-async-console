#include "async_console.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/fcntl.h>
#include <unistd.h>

#include "async_console_io.h"
#include "async_console_log.h"
#include "driver/uart.h"
#include "driver/uart_vfs.h"
#include "esp_console.h"
#include "esp_err.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "linenoise/async_linenoise.h"

static const char *TAG = "async_console";
static char s_prompt[64];
static uart_port_t s_uart_num = UART_NUM_0;

static void repl_edit_start(void) {
  int in_fd = fileno(stdin);
  int flags = fcntl(in_fd, F_GETFL);
  fcntl(in_fd, F_SETFL, flags | O_NONBLOCK);

  while (linenoiseEditStart(&g_ls, g_ln_buf, sizeof(g_ln_buf), s_prompt) != 0) {
    vTaskDelay(pdMS_TO_TICKS(100));
  }

  vTaskDelay(pdMS_TO_TICKS(20));
  uart_flush_input(s_uart_num);
}

static void console_repl_task(void *arg) {
  (void)arg;

  linenoiseSetMultiLine(0);
  linenoiseHistorySetMaxLen(20);
  linenoiseSetCompletionCallback(esp_console_get_completion);
  linenoiseSetHintsCallback(esp_console_get_hint);

  vTaskDelay(pdMS_TO_TICKS(100));

  // Start in auto, user can manually force smart or dumb mode.
  linenoiseSetTerminalMode(LINENOISE_MODE_AUTO);

  repl_edit_start();

  while (1) {
    char *line = linenoiseEditFeed(&g_ls);

    if (line == linenoiseEditMore) {
      vTaskDelay(pdMS_TO_TICKS(10));
      continue;
    }

    linenoiseEditStop(&g_ls);

    if (line == NULL) {
      repl_edit_start();
      continue;
    }

    if (line[0] != '\0') {
      linenoiseHistoryAdd(line);

      int ret;
      esp_err_t err = esp_console_run(line, &ret);
      if (err == ESP_ERR_NOT_FOUND) {
        printf("Unknown command. Type 'help' for a list.\n");
      } else if (err == ESP_ERR_INVALID_ARG) {
        /* Blank / whitespace-only line — ignore silently. */
      } else if (err != ESP_OK) {
        printf("Command error: %s\n", esp_err_to_name(err));
      }
    }

    linenoiseFree(line);
    repl_edit_start();
  }
}

esp_err_t async_console_init(uart_port_t uart_num, int baud_rate, const char *prompt) {
  esp_err_t ret;

  s_uart_num = uart_num;

  if (prompt) {
    strlcpy(s_prompt, prompt, sizeof(s_prompt));
  } else {
    strlcpy(s_prompt, "esp32> ", sizeof(s_prompt));
  }

  fflush(stdout);
  fsync(fileno(stdout));

  uart_vfs_dev_port_set_rx_line_endings(uart_num, ESP_LINE_ENDINGS_CR);
  uart_vfs_dev_port_set_tx_line_endings(uart_num, ESP_LINE_ENDINGS_CRLF);

  fcntl(fileno(stdout), F_SETFL, 0);
  fcntl(fileno(stdin),  F_SETFL, 0);

  const uart_config_t uart_cfg = {
      .baud_rate  = baud_rate,
      .data_bits  = UART_DATA_8_BITS,
      .parity     = UART_PARITY_DISABLE,
      .stop_bits  = UART_STOP_BITS_1,
      .source_clk = UART_SCLK_DEFAULT,
  };
  uart_param_config(uart_num, &uart_cfg);

  ret = uart_driver_install(uart_num, 256, 0, 0, NULL, 0);
  if (ret != ESP_OK && ret != ESP_ERR_INVALID_STATE) {
    ESP_LOGE(TAG, "UART driver install failed: %s", esp_err_to_name(ret));
    return ret;
  }

  uart_vfs_dev_use_driver(uart_num);

  const esp_console_config_t console_cfg = {
      .max_cmdline_args   = CONFIG_ASYNC_CONSOLE_MAX_CMDLINE_ARGS,
      .max_cmdline_length = CONFIG_ASYNC_CONSOLE_MAX_CMDLINE_LENGTH,
  };
  ret = esp_console_init(&console_cfg);
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "esp_console_init failed: %s", esp_err_to_name(ret));
    return ret;
  }

  esp_console_register_help_command();

  async_console_io_init();
  async_console_log_hook_install();

  if (xTaskCreate(console_repl_task, "console_repl", 6144, NULL, 5, NULL) != pdTRUE) {
    ESP_LOGE(TAG, "Failed to create REPL task");
    return ESP_FAIL;
  }

  return ESP_OK;
}
