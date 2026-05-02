add_library(stm32_sensors INTERFACE)

target_include_directories(stm32_sensors INTERFACE
    ${_STM32_SDK_DIR}/sensors/include
    ${_STM32_SDK_DIR}/sensors/accelerometers/mpu6050
)

target_link_libraries(stm32_sensors INTERFACE stm32_drivers)

if(STM32_USE_FREERTOS)
    target_compile_definitions(stm32_sensors INTERFACE STM32_USE_FREERTOS)
endif()
