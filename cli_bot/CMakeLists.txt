cmake_minimum_required (VERSION 3.16.3)

project(cli_bot)

set(CMAKE_CXX_FLAGS                "-Wall -Werror -Wextra -pedantic -pedantic-errors -Wconversion -Wsign-conversion -Werror=pedantic")
set(CMAKE_CXX_STANDARD 17)
set(COMPILE_WARNING_MODE HIGH)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -fprofile-arcs -ftest-coverage")
set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -fprofile-arcs -ftest-coverage")

set(bot_src 
	main.cpp
    udp_handler.cpp
    blue_boot.cpp
)

add_executable(blue_boot ${bot_src})
