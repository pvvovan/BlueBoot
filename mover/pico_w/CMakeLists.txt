cmake_minimum_required(VERSION 3.12)

set(PICO_BOARD pico_w)

# Pull in SDK (must be before project)
include(pico_sdk_import.cmake)

project(pico_blink C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

add_compile_options(-Wall -Wextra)

pico_sdk_init()

add_executable(pico_mover
	picomover.cpp
	../move_cmd.c
	hwioab.cpp
)

target_include_directories(pico_mover PRIVATE
	${CMAKE_CURRENT_LIST_DIR}
	${CMAKE_CURRENT_LIST_DIR}/..
)

# pull in common dependencies
target_link_libraries(pico_mover
	pico_stdlib
	hardware_pwm
	pico_cyw43_arch_lwip_poll
)

# create map/bin/hex file etc.
pico_add_extra_outputs(pico_mover)
