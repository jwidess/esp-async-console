#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_console.h"
#include "nvs_flash.h"
#include "driver/uart.h"
#include "async_console.h"

static const char *TAG = "main";

static int cmd_hello(int argc, char **argv)
{
    printf("Hello from async console!\n");
    return 0;
}

static int cmd_echo(int argc, char **argv)
{
    for (int i = 1; i < argc; i++) {
        printf("%s ", argv[i]);
    }
    printf("\n");
    return 0;
}

static int cmd_table(int argc, char **argv)
{
    printf("+-------+----------------------+-------+\n");
    printf("| ID    | Sensor               | Value |\n");
    printf("+-------+----------------------+-------+\n");
    for (int i = 0; i < 20; i++) {
        printf("| %-5d | Temperature Sensor %-2d | %-5.2f |\n", i, i, 20.0 + (i * 0.5));
    }
    printf("+-------+----------------------+-------+\n");
    return 0;
}

static void background_log_task(void *arg)
{
    int tick = 0;
    while (1) {
        ESP_LOGI("bg", "tick %d", tick++);
        
        if (tick % 5 == 0) {
            ESP_LOGI("bg_table", "\n"
                     "+-------+----------------------+\n"
                     "| ID    | Status               |\n"
                     "+-------+----------------------+\n"
                     "| 0     | OK                   |\n"
                     "| 1     | WARNING              |\n"
                     "| 2     | ERROR                |\n"
                     "+-------+----------------------+");
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void app_main(void)
{
    ESP_LOGI(TAG, "Starting basic_repl example");

    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);

    /* Initialize the async console */
    ESP_ERROR_CHECK(async_console_init(UART_NUM_0, 115200, "esp32> "));

    /* Register test commands */
    const esp_console_cmd_t hello_cmd = {
        .command = "hello",
        .help = "Say hello",
        .hint = NULL,
        .func = &cmd_hello,
        .argtable = NULL
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&hello_cmd));

    const esp_console_cmd_t echo_cmd = {
        .command = "echo",
        .help = "Echo arguments",
        .hint = "<args>",
        .func = &cmd_echo,
        .argtable = NULL
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&echo_cmd));

    const esp_console_cmd_t table_cmd = {
        .command = "table",
        .help = "Print a large table of mock data",
        .hint = NULL,
        .func = &cmd_table,
        .argtable = NULL
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&table_cmd));

    /* Start background task to test log interleaving */
    xTaskCreate(background_log_task, "bg_log", 2048, NULL, 5, NULL);

    ESP_LOGI(TAG, "Initialization complete. Try typing 'hello', 'echo 123', or 'table'");
}
