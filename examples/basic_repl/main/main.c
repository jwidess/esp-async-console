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
#include "linenoise/async_linenoise.h"

#define BG_LOG_TASK_DELAY_MS 3000
#define BG_LOG_TABLE_INTERVAL 10

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

static int cmd_term_mode(int argc, char **argv)
{
    if (argc > 1) {
        if (strcmp(argv[1], "auto") == 0) {
            linenoiseSetTerminalMode(LINENOISE_MODE_AUTO);
            printf("Terminal mode set to AUTO.\n");
        } else if (strcmp(argv[1], "smart") == 0) {
            linenoiseSetTerminalMode(LINENOISE_MODE_SMART);
            printf("Terminal mode set to SMART.\n");
        } else if (strcmp(argv[1], "dumb") == 0) {
            linenoiseSetTerminalMode(LINENOISE_MODE_DUMB);
            printf("Terminal mode set to DUMB.\n");
        } else {
            printf("Usage: terminalmode [auto|smart|dumb]\n");
        }
    } else {
        linenoiseTerminalMode_t current = linenoiseGetTerminalMode();
        const char *mode_str = "AUTO";
        if (current == LINENOISE_MODE_SMART) mode_str = "SMART";
        else if (current == LINENOISE_MODE_DUMB) mode_str = "DUMB";
        
        printf("Current terminal mode: %s\n", mode_str);
        printf("Usage: terminalmode [auto|smart|dumb]\n");
    }
    return 0;
}

static bool current_debug_mode = false;
static int cmd_debugmode(int argc, char **argv)
{
    if (argc > 1) {
        if (strcmp(argv[1], "on") == 0 || strcmp(argv[1], "1") == 0) {
            current_debug_mode = true;
            esp_console_set_debug_mode(true);
            printf("Debug mode enabled.\n");
        } else if (strcmp(argv[1], "off") == 0 || strcmp(argv[1], "0") == 0) {
            current_debug_mode = false;
            esp_console_set_debug_mode(false);
            printf("Debug mode disabled.\n");
        } else {
            printf("Usage: debugmode [on|off]\n");
        }
    } else {
        /* Toggle */
        current_debug_mode = !current_debug_mode;
        esp_console_set_debug_mode(current_debug_mode);
        printf("Debug mode %s.\n", current_debug_mode ? "enabled" : "disabled");
    }
    return 0;
}

static int cmd_reboot(int argc, char **argv)
{
    printf("Rebooting...\n");
    esp_restart();
    return 0;
}

static void background_log_task(void *arg)
{
    int tick = 0;
    while (1) {
        ESP_LOGI("bg", "tick %d", tick++);
        
        if (tick % BG_LOG_TABLE_INTERVAL == 0) {
            ESP_LOGI("bg_table", "\n"
                     "+-------+----------------------+\n"
                     "| ID    | Status               |\n"
                     "+-------+----------------------+\n"
                     "| 0     | OK                   |\n"
                     "| 1     | WARNING              |\n"
                     "| 2     | ERROR                |\n"
                     "+-------+----------------------+");
        }
        vTaskDelay(pdMS_TO_TICKS(BG_LOG_TASK_DELAY_MS));
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
    ESP_ERROR_CHECK(async_console_init(UART_NUM_0, CONFIG_ESP_CONSOLE_UART_BAUDRATE, "esp32> "));

    //esp_console_set_debug_mode(true); // Enable debug mode by default

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

    const esp_console_cmd_t term_cmd = {
        .command = "terminalmode",
        .help = "Set terminal capability detection (or 'terminalmode auto/smart/dumb')",
        .hint = "[auto|smart|dumb]",
        .func = &cmd_term_mode,
        .argtable = NULL
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&term_cmd));

    const esp_console_cmd_t debugmode_cmd = {
        .command = "debugmode",
        .help = "Toggle debug mode (or 'debugmode on/off')",
        .hint = "[on|off]",
        .func = &cmd_debugmode,
        .argtable = NULL
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&debugmode_cmd));
    
    const esp_console_cmd_t reboot_cmd = {
        .command = "reboot",
        .help = "Reboot the chip",
        .hint = NULL,
        .func = &cmd_reboot,
        .argtable = NULL
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&reboot_cmd));

    /* Start background task to test log interleaving */
    xTaskCreate(background_log_task, "bg_log", 2048, NULL, 5, NULL);

    ESP_LOGI(TAG, "Initialization complete. Try typing 'hello', 'echo 123', or 'table'");
}
