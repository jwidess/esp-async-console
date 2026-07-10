#pragma once

#include "esp_err.h"
#include "driver/uart.h"

/**
 * @brief Initialize and start the asynchronous serial console task.
 *
 * @param uart_num   UART port number to use (e.g., UART_NUM_0)
 * @param baud_rate  UART baud rate
 * @param prompt     The prompt string (e.g., "esp32> ")
 * @return ESP_OK on success.
 */
esp_err_t async_console_init(uart_port_t uart_num, int baud_rate, const char *prompt);
