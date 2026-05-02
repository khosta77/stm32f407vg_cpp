add_library(stm32_drivers OBJECT)

target_sources(stm32_drivers PUBLIC
    FILE_SET CXX_MODULES FILES
        ${_STM32_SDK_DIR}/drivers/include/driver/types.cppm
        ${_STM32_SDK_DIR}/drivers/include/driver/reg.cppm
        ${_STM32_SDK_DIR}/drivers/include/driver/circular_buffer.cppm
        ${_STM32_SDK_DIR}/drivers/include/driver/gpio.cppm
        ${_STM32_SDK_DIR}/drivers/include/driver/uart.cppm
        ${_STM32_SDK_DIR}/drivers/include/driver/i2c.cppm
        ${_STM32_SDK_DIR}/drivers/include/driver/spi.cppm
        ${_STM32_SDK_DIR}/drivers/include/driver/flash.cppm
)

target_include_directories(stm32_drivers PUBLIC
    ${_STM32_SDK_DIR}/drivers/include
)

target_compile_features(stm32_drivers PUBLIC cxx_std_20)

target_compile_options(stm32_drivers PRIVATE
    ${STM32_ARCH_FLAGS}
    -Os
    -ffreestanding
    -ffunction-sections
    -fdata-sections
)

target_compile_definitions(stm32_drivers PRIVATE
    ${STM32_DEFINE}
)

if(STM32_USE_FREERTOS)
    target_compile_definitions(stm32_drivers PUBLIC STM32_USE_FREERTOS)
endif()
