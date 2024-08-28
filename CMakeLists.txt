cmake_minimum_required(VERSION 3.13)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(pico_sdk_import.cmake)

project(pico-sx126x C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
file(GLOB_RECURSE SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "src/*.[ch]")

pico_sdk_init()

add_library(pico-sx126x
    ${SOURCES}
)

target_include_directories(pico-sx126x
    PUBLIC 
    include
    "${CMAKE_CURRENT_SOURCE_DIR}"
)


target_link_libraries(pico-sx126x
    pico_stdlib
    pico_multicore
    hardware_irq
    hardware_spi
)
