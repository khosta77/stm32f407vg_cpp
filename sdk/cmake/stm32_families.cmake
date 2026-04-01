macro(stm32_resolve_family CHIP)
    string(TOUPPER ${CHIP} _CHIP_UPPER)
    string(SUBSTRING "${_CHIP_UPPER}" 5 2 _FAMILY_PREFIX)
    string(TOLOWER "${_FAMILY_PREFIX}" _FAMILY_LOWER)

    set(_FAMILY_FILE "${CMAKE_CURRENT_LIST_DIR}/families/stm32${_FAMILY_LOWER}.cmake")

    if(NOT EXISTS "${_FAMILY_FILE}")
        message(FATAL_ERROR "Unsupported STM32 family: ${_FAMILY_PREFIX} (chip: ${CHIP})\n"
            "No family file found at: ${_FAMILY_FILE}")
    endif()

    include("${_FAMILY_FILE}")
    cmake_language(CALL "stm32${_FAMILY_LOWER}_get_chip_info" "${CHIP}")
endmacro()
