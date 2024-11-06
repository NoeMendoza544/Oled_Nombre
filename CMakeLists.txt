
cmake_minimum_required(VERSION 3.12)

set(PICO_BOARD pico_w)

# Pull in SDK (must be before project)
include(pico_sdk_import.cmake)

project(proyectos-pico)


# Initialize the SDK
pico_sdk_init()


add_executable(NOMB
        NOMB.c
        )

target_include_directories(NOMB PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/../wifi # for our common lwipopts
        )



# create map/bin/hex file etc.
# pico_enable_stdio_usb(picow_blink 1)
# create map/bin/hex file etc.
pico_add_extra_outputs(NOMB)




target_link_libraries(NOMB
        pico_stdlib
	hardware_i2c
        )

pico_enable_stdio_usb(NOMB 1)
pico_enable_stdio_uart(NOMB 0)