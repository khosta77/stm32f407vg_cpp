if(NOT DEFINED STM32_CHIP)
    message(FATAL_ERROR "STM32_CHIP is not defined. Set -DSTM32_CHIP=STM32F407VG (or similar)")
endif()

include(${CMAKE_CURRENT_LIST_DIR}/stm32_families.cmake)
stm32_resolve_family(${STM32_CHIP})
