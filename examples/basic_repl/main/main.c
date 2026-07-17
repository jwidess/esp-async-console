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

#define BG_LOG_TASK_DELAY_MS 2000
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

static int cmd_mode_msgs(int argc, char **argv)
{
    bool current = linenoiseGetModeMessages();
    linenoiseSetModeMessages(!current);
    printf("Terminal mode change messages %s.\n", !current ? "enabled" : "disabled");
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

static int cmd_delay(int argc, char **argv)
{
    int ms = 5000;
    if (argc > 1) {
        ms = atoi(argv[1]);
    }
    printf("Delaying for %d ms. Watch background logs...\n", ms);
    vTaskDelay(pdMS_TO_TICKS(ms));
    printf("Delay complete.\n");
    return 0;
}

static int cmd_logtest(int argc, char **argv)
{
    ESP_LOGE("logtest", "This is an error");
    ESP_LOGW("logtest", "This is a warning");
    ESP_LOGI("logtest", "This is an info message");
    ESP_LOGD("logtest", "This is a debug message");
    ESP_LOGV("logtest", "This is a verbose message");
    
    printf("\n\033[1;36mThis is custom bold cyan text!\033[0m\n");
    printf("\033[4;35mThis is custom underlined magenta text!\033[0m\n\n");

    printf("This is raw stdout text without newline. Delaying for 1.5 seconds...");
    fflush(stdout);
    vTaskDelay(pdMS_TO_TICKS(1500));
    printf(" and this is the rest of the line.\n");
    return 0;
}

static void background_log_task(void *arg)
{
    int tick = 0;
    
    vTaskDelay(pdMS_TO_TICKS(200));
    
    while (1) {
        /* Cycle through different log levels */
        switch (tick % 5) {
            case 0: ESP_LOGE("bg", "Error log example (tick %d)", tick); break;
            case 1: ESP_LOGW("bg", "Warning log example (tick %d)", tick); break;
            case 2: ESP_LOGI("bg", "Info log example (tick %d)", tick); break;
            case 3: ESP_LOGD("bg", "Debug log example (tick %d)", tick); break;
            case 4: ESP_LOGV("bg", "Verbose log example (tick %d)", tick); break;
        }
        tick++;
        
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

// This overrides the standard ESP-IDF console hints with custom colors
static const char *custom_hints(const char *buf, int *color, int *bold) {
    // Get the standard ESP-IDF hint
    const char *hint = esp_console_get_hint(buf, color, bold);
    
    if (hint) {
        // Override hint color based on the command being typed
        if (strncmp(buf, "terminalmode", 12) == 0) {
            *color = 35; // Magenta
            *bold = 1;
        } else if (strncmp(buf, "debugmode", 9) == 0) {
            *color = 33; // Yellow
            *bold = 0;
        } else {
            *color = 36; // Cyan (Default for other commands)
            *bold = 0;
        }
    }
    return hint;
}

static void custom_completion(const char *buf, linenoiseCompletions *lc) {
    // Fallback to standard ESP-IDF console command completions
    esp_console_get_completion(buf, lc);
    
    // Add custom argument completions for `terminalmode`
    if (strncmp(buf, "terminalmode ", 13) == 0) {
        const char *arg = buf + 13;
        if (strncmp(arg, "a", 1) == 0 || strlen(arg) == 0) {
            linenoiseAddCompletion(lc, "terminalmode auto");
        }
        if (strncmp(arg, "s", 1) == 0 || strlen(arg) == 0) {
            linenoiseAddCompletion(lc, "terminalmode smart");
        }
        if (strncmp(arg, "d", 1) == 0 || strlen(arg) == 0) {
            linenoiseAddCompletion(lc, "terminalmode dumb");
        }
    }
    
    // Add custom argument completions for `debugmode`
    if (strncmp(buf, "debugmode ", 10) == 0) {
        const char *arg = buf + 10;
        if (strncmp(arg, "o", 1) == 0 || strlen(arg) == 0) {
            linenoiseAddCompletion(lc, "debugmode on");
            linenoiseAddCompletion(lc, "debugmode off");
        }
    }
}

void app_main(void)
{
    ESP_LOGI(TAG, "================================");
    ESP_LOGI(TAG, "Starting basic_repl example");

    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);

    /* Initialize the async console */
    ESP_ERROR_CHECK(async_console_init(UART_NUM_0, CONFIG_ESP_CONSOLE_UART_BAUDRATE, "esp32> "));
    
    /* Optional: Override standard completion and hints to add custom behaviors */
    linenoiseSetCompletionCallback(custom_completion);
    linenoiseSetHintsCallback(custom_hints);

    esp_log_level_set("bg", ESP_LOG_VERBOSE);
    esp_log_level_set("logtest", ESP_LOG_VERBOSE);

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

    const esp_console_cmd_t mode_msgs_cmd = {
        .command = "modemessages",
        .help = "Toggle terminal mode change messages",
        .hint = NULL,
        .func = &cmd_mode_msgs,
        .argtable = NULL
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&mode_msgs_cmd));

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

    const esp_console_cmd_t delay_cmd = {
        .command = "delay",
        .help = "Delay for a given number of milliseconds (demos background logging while blocked)",
        .hint = "<ms>",
        .func = &cmd_delay,
        .argtable = NULL
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&delay_cmd));

    const esp_console_cmd_t logtest_cmd = {
        .command = "logtest",
        .help = "Emit one of each ESP_LOG level and demo raw stdout",
        .hint = NULL,
        .func = &cmd_logtest,
        .argtable = NULL
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&logtest_cmd));

    /* Start background task to test log interleaving */
    xTaskCreate(background_log_task, "bg_log", 2048, NULL, 5, NULL);

    ESP_LOGI(TAG, "Init complete. Try typing 'hello', 'echo 123', or 'logtest'");
    ESP_LOGI(TAG, "================================");
}
