add_library(stm32_drivers STATIC
    ${_STM32_SDK_DIR}/drivers/src/stm32f4/gpio.cpp
    ${_STM32_SDK_DIR}/drivers/src/stm32f4/uart.cpp
    ${_STM32_SDK_DIR}/drivers/src/stm32f4/i2c.cpp
    ${_STM32_SDK_DIR}/drivers/src/stm32f4/spi.cpp
    ${_STM32_SDK_DIR}/drivers/src/stm32f4/flash.cpp
)

target_include_directories(stm32_drivers PUBLIC
    ${_STM32_SDK_DIR}/drivers/include
)

target_include_directories(stm32_drivers PRIVATE
    ${_STM32_SDK_DIR}/core/include
    ${_STM32_SDK_DIR}/core/include/cmsis
    ${STM32_HAL_DIR}/include
    ${STM32_HAL_DIR}/include/cmsis
)

target_compile_options(stm32_drivers PRIVATE
    ${STM32_ARCH_FLAGS}
    -Os
    -ffreestanding
    -ffunction-sections
    -fdata-sections
    $<$<COMPILE_LANGUAGE:CXX>:-std=gnu++17>
    $<$<COMPILE_LANGUAGE:C>:-std=gnu11>
)

target_compile_definitions(stm32_drivers PRIVATE
    ${STM32_DEFINE}
)

if(STM32_USE_FREERTOS)
    target_compile_definitions(stm32_drivers PRIVATE STM32_USE_FREERTOS)
    target_include_directories(stm32_drivers PRIVATE
        ${freertos_kernel_SOURCE_DIR}/include
        ${_FREERTOS_PORT_DIR}
        ${_STM32_SDK_DIR}/rtos/include
        ${_STM32_SDK_DIR}/rtos/include/rtos
    )
    target_link_libraries(stm32_drivers PUBLIC stm32_rtos)
endif()
