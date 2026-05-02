add_library(stm32_drivers INTERFACE)

target_include_directories(stm32_drivers INTERFACE
    ${_STM32_SDK_DIR}/drivers/include
)

if(STM32_USE_FREERTOS)
    target_compile_definitions(stm32_drivers INTERFACE STM32_USE_FREERTOS)
endif()
