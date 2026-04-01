function(stm32u5_get_chip_info CHIP)
    message(FATAL_ERROR
        "STM32U5 family is not yet supported by stm32-sdk.\n"
        "Chip requested: ${CHIP}\n"
        "Currently supported families: F4\n"
        "Contributions welcome: https://github.com/khosta77/stm32-sdk")
endfunction()
