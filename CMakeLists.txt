cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

# include(pico_extras_import.cmake)

# project(pico_playground C CXX)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

project(main)

pico_sdk_init()

add_executable(main
        main.c
        usb_descriptors.c
        )

        target_include_directories(main PRIVATE ${CMAKE_CURRENT_LIST_DIR})

target_link_libraries(main PRIVATE pico_stdlib tinyusb_device tinyusb_board hardware_pwm pico_cyw43_arch_lwip_threadsafe_background)
# tinyusb_device tinyusb_board

# pico_enable_stdio_usb(main 1)
# pico_enable_stdio_uart(main 0)

pico_enable_stdio_uart(main 1)

pico_add_extra_outputs(main)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Og -DCFG_TUSB_DEBUG=2")
set(CMAKE_DEFSYM "${CMAKE_DEFSYM} -DLOG=2")
