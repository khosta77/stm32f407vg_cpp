add_library(stm32_storage INTERFACE)

target_include_directories(stm32_storage INTERFACE
    ${_STM32_SDK_DIR}/storage/include
)

target_link_libraries(stm32_storage INTERFACE stm32_drivers)
