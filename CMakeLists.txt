cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

if (PLATFORM STREQUAL "mbed-os")
    add_library(mk-hsm INTERFACE)

    if (PORT STREQUAL "STM32F429ZI")

    endif ()

    target_include_directories(drivers INTERFACE
            ${CMAKE_CURRENT_SOURCE_DIR}/include/
    )
    target_sources(drivers INTERFACE
            ${CMAKE_CURRENT_SOURCE_DIR}/ports/mbed-common/fsm.cpp
    )
else ()
    idf_component_register(
            SRCS
            "hsm.cpp"
            "ports/ESP32/ESP32_fsm.cpp"
            INCLUDE_DIRS
            "./include"
            "ports/ESP32/include"
            REQUIRES
            "driver"
            "drivers"
            "log"
    )
endif ()
