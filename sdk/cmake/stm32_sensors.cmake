add_library(stm32_sensors OBJECT)

target_sources(stm32_sensors PUBLIC
    FILE_SET CXX_MODULES
    BASE_DIRS ${_STM32_SDK_DIR}
    FILES
        ${_STM32_SDK_DIR}/sensors/include/sensor/imu.cppm
        ${_STM32_SDK_DIR}/sensors/accelerometers/mpu6050/mpu6050.cppm
)

target_include_directories(stm32_sensors PUBLIC
    ${_STM32_SDK_DIR}/sensors/include
    ${_STM32_SDK_DIR}/sensors/accelerometers/mpu6050
)

target_compile_features(stm32_sensors PUBLIC cxx_std_20)

target_compile_options(stm32_sensors PRIVATE
    ${STM32_ARCH_FLAGS}
    -Os
    -ffreestanding
    -ffunction-sections
    -fdata-sections
)

target_compile_definitions(stm32_sensors PRIVATE
    ${STM32_DEFINE}
)

target_link_libraries(stm32_sensors PUBLIC stm32_drivers)

if(STM32_USE_FREERTOS)
    target_compile_definitions(stm32_sensors PUBLIC STM32_USE_FREERTOS)
endif()
