# stm32f4_template

Bare-metal C++17 шаблон для микроконтроллеров серии STM32F4. Только CMSIS, без HAL/LL.

## Требования

- `arm-none-eabi-gcc` >= 12
- `cmake` >= 3.20
- `st-flash` (опционально, для прошивки через ST-Link)

## Быстрый старт

```bash
cmake -B build -DSTM32_CHIP=STM32F407VG
cmake --build build
```

## Прошивка

```bash
cmake --build build --target flash
```

Или вручную:

```bash
st-flash --reset write build/stm32f4_template.bin 0x08000000
```

## Выбор чипа

```bash
cmake -B build -DSTM32_CHIP=STM32F411CE
```

Поддерживаемые семейства:

| Семейство | RAM | CCM | Flash | Пример чипа |
|-----------|-----|-----|-------|-------------|
| STM32F401 | 64-96K | - | 128-512K | STM32F401CC, STM32F401RE |
| STM32F405 | 128K | 64K | 512-1024K | STM32F405RG |
| STM32F407 | 128K | 64K | 512-1024K | STM32F407VG |
| STM32F411 | 128K | - | 256-512K | STM32F411CE |
| STM32F412 | 256K | - | 512-1024K | STM32F412VG |
| STM32F429 | 192K | 64K | 512-2048K | STM32F429ZI |
| STM32F439 | 192K | 64K | 512-2048K | STM32F439ZI |
| STM32F446 | 128K | - | 256-512K | STM32F446RE |

Размер Flash определяется автоматически из имени чипа (буква в позиции 11: B=128K, C=256K, E=512K, G=1024K, I=2048K).

## Структура проекта

```
.
├── CMakeLists.txt
├── cmake/
│   ├── arm-none-eabi.cmake      # Toolchain для кросс-компиляции
│   └── stm32f4_chips.cmake      # База данных чипов STM32F4
├── ldscripts/
│   ├── mem.ld.in                # Шаблон карты памяти (parameterized)
│   ├── sections.ld              # Разделы линковки
│   └── libs.ld                  # Подключение библиотек
├── src/
│   └── main.cpp                 # Точка входа (LED blink для Discovery)
├── system/
│   ├── include/
│   │   ├── cmsis/               # CMSIS headers для STM32F4
│   │   ├── cortexm/             # Обработчики исключений Cortex-M
│   │   ├── diag/                # Trace/отладка
│   │   └── arm/                 # ARM semihosting
│   └── src/
│       ├── cmsis/               # System init + vector tables
│       ├── cortexm/             # Hardware init, exception handlers
│       ├── diag/                # Trace implementation
│       └── newlib/              # Newlib integration (startup, syscalls)
└── docs/
    └── DATASHEETS.md            # Ссылки на документацию ST
```

## Лицензия

MIT
