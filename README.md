# ESP Async Console

A non-blocking async REPL console for ESP-IDF. It allows `ESP_LOG` messages to cleanly interleave with the active serial console prompt without screen corruption.

This component is built on a fork of the [ESP-IDF Console component](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/system/console.html), primarily re-writing `linenoise` as a character-feed state machine so the console REPL runs in a FreeRTOS task without blocking on `read()`.

## License

This project is licensed under the Apache License 2.0.
