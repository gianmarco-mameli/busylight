cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

project(main)

pico_sdk_init()


add_executable(main
        main.c
        usb_descriptors.c
        )

pico_generate_pio_header(main ${CMAKE_CURRENT_LIST_DIR}/ws2812.pio OUTPUT_DIR ${CMAKE_CURRENT_LIST_DIR}/build/generated)

target_include_directories(main PRIVATE ${CMAKE_CURRENT_LIST_DIR})

target_link_libraries(main PRIVATE pico_stdlib tinyusb_device tinyusb_board hardware_pwm hardware_pio)

pico_enable_stdio_uart(main 1)

pico_add_extra_outputs(main)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Og -DCFG_TUSB_DEBUG=2")
set(CMAKE_DEFSYM "${CMAKE_DEFSYM} -DLOG=2")
