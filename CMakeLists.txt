cmake_minimum_required(VERSION 3.13)
include(pico_sdk_import.cmake)
project(radio_project C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
pico_sdk_init()
add_executable(radio radio.c )

# Bibliotheksverzeichnisse
add_subdirectory(./buttons)
add_subdirectory(./ili_9341)
add_subdirectory(./si4703)

target_link_libraries(radio
	buttons
	ili_9341
	si4703
	pico_stdlib)
pico_add_extra_outputs(radio)
