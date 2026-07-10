idf_build_get_property(target IDF_TARGET)

set(srcs "linenoise/async_linenoise.c"
         "async_console_io.c"
         "async_console_log.c"
         "async_console.c")

set(requires console vfs)

if(${target} STREQUAL "linux")
else()
    list(APPEND requires esp_stdio)
endif()

idf_component_register(SRCS ${srcs}
                    INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
                    REQUIRES "${requires}"
                    PRIV_REQUIRES esp_driver_uart
                                  esp_driver_usb_serial_jtag
                                  esp_usb_cdc_rom_console
                    )


if(CONFIG_COMPILER_STATIC_ANALYZER AND CMAKE_C_COMPILER_ID STREQUAL "GNU") # TODO IDF-10085
    target_compile_options(${COMPONENT_LIB} PRIVATE "-fno-analyzer")
endif()
