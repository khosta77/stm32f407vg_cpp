cmake_minimum_required(VERSION 3.20)

if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE
        ${CMAKE_CURRENT_SOURCE_DIR}/cmake/arm-none-eabi.cmake
        CACHE FILEPATH "Toolchain file")
endif()

project(stm32f4_template LANGUAGES C CXX)

include(cmake/stm32f4_chips.cmake)

set(STM32_CHIP "STM32F407VG" CACHE STRING "Target STM32F4 chip (e.g. STM32F407VG, STM32F411CE)")
stm32f4_get_chip_info(${STM32_CHIP})

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/ldscripts)
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/sdk/hal/stm32f4/ldscripts/mem.ld.in
    ${CMAKE_CURRENT_BINARY_DIR}/ldscripts/mem.ld
    @ONLY
)

set(APP_SOURCES
    src/main.cpp
)

set(CMSIS_SOURCES
    sdk/hal/stm32f4/src/cmsis/system_stm32f4xx.c
    sdk/hal/stm32f4/src/cmsis/${STM32_VECTORS_FILE}
)

set(CORTEXM_SOURCES
    sdk/core/src/cortexm/exception-handlers.c
    sdk/core/src/cortexm/initialize-hardware.c
    sdk/core/src/cortexm/reset-hardware.c
)

set(DIAG_SOURCES
    sdk/core/src/diag/trace-impl.c
    sdk/core/src/diag/trace.c
)

set(NEWLIB_C_SOURCES
    sdk/core/src/newlib/assert.c
    sdk/core/src/newlib/exit.c
    sdk/core/src/newlib/sbrk.c
    sdk/core/src/newlib/startup.c
    sdk/core/src/newlib/syscalls.c
)

set(NEWLIB_CXX_SOURCES
    sdk/core/src/newlib/cxx.cpp
)

add_executable(${PROJECT_NAME}.elf
    ${APP_SOURCES}
    ${CMSIS_SOURCES}
    ${CORTEXM_SOURCES}
    ${DIAG_SOURCES}
    ${NEWLIB_C_SOURCES}
    ${NEWLIB_CXX_SOURCES}
)

target_compile_options(${PROJECT_NAME}.elf PRIVATE
    -mcpu=cortex-m4
    -mthumb
    -mfloat-abi=soft
    -O0
    -fmessage-length=0
    -fsigned-char
    -ffunction-sections
    -fdata-sections
    -ffreestanding
    -fno-move-loop-invariants
    -g3
    $<$<COMPILE_LANGUAGE:C>:-std=gnu11>
    $<$<COMPILE_LANGUAGE:CXX>:-std=gnu++17>
)

target_compile_definitions(${PROJECT_NAME}.elf PRIVATE
    DEBUG
    TRACE
    OS_USE_TRACE_SEMIHOSTING_STDOUT
    ${STM32_DEFINE}
    HSE_VALUE=8000000
)

target_include_directories(${PROJECT_NAME}.elf PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/sdk/core/include
    ${CMAKE_CURRENT_SOURCE_DIR}/sdk/core/include/cmsis
    ${CMAKE_CURRENT_SOURCE_DIR}/sdk/hal/stm32f4/include
    ${CMAKE_CURRENT_SOURCE_DIR}/sdk/hal/stm32f4/include/cmsis
)

set_source_files_properties(
    sdk/hal/stm32f4/src/cmsis/system_stm32f4xx.c
    PROPERTIES COMPILE_OPTIONS "-Wno-padded"
)

set_source_files_properties(
    sdk/core/src/newlib/startup.c
    PROPERTIES COMPILE_DEFINITIONS "OS_INCLUDE_STARTUP_INIT_MULTIPLE_RAM_SECTIONS"
)

set_source_files_properties(
    sdk/core/src/newlib/cxx.cpp
    PROPERTIES COMPILE_OPTIONS
        "-std=gnu++11;-fabi-version=0;-fno-exceptions;-fno-rtti;-fno-use-cxa-atexit;-fno-threadsafe-statics"
)

set(LINKER_SCRIPT_DIR_GEN ${CMAKE_CURRENT_BINARY_DIR}/ldscripts)
set(LINKER_SCRIPT_DIR_SRC ${CMAKE_CURRENT_SOURCE_DIR}/sdk/core/ldscripts)

target_link_options(${PROJECT_NAME}.elf PRIVATE
    -mcpu=cortex-m4
    -mthumb
    -mfloat-abi=soft
    -T${LINKER_SCRIPT_DIR_GEN}/mem.ld
    -T${LINKER_SCRIPT_DIR_SRC}/libs.ld
    -T${LINKER_SCRIPT_DIR_SRC}/sections.ld
    --specs=nano.specs
    -Xlinker --gc-sections
    -Wl,--no-warn-rwx-segments
    -Wl,--print-memory-usage
    -Wl,-Map,${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.map
)

add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O ihex
        $<TARGET_FILE:${PROJECT_NAME}.elf>
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.hex
    COMMAND ${CMAKE_OBJCOPY} -O binary
        $<TARGET_FILE:${PROJECT_NAME}.elf>
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.bin
)

add_custom_target(flash
    COMMAND st-flash --reset write
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.bin 0x08000000
    DEPENDS ${PROJECT_NAME}.elf
)
