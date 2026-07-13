#pragma once

#include <stdbool.h>
#include "esp_err.h"
#include "driver/uart.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize and start the asynchronous serial console task.
 *
 * @param uart_num   UART port number to use (e.g., UART_NUM_0)
 * @param baud_rate  UART baud rate
 * @param prompt     The prompt string (e.g., "esp32> ")
 * @return ESP_OK on success.
 */
esp_err_t async_console_init(uart_port_t uart_num, int baud_rate, const char *prompt);

/**
 * @brief Enable or disable raw RX debug logging (useful for debugging terminal escapes).
 *
 * @param enable true to enable debug logs, false to disable.
 */
void esp_console_set_debug_mode(bool enable);

#ifdef __cplusplus
}
#endif
