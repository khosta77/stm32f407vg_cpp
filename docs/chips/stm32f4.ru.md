# Семейство STM32F4

Опорное семейство SDK. Все драйверы и шаблоны валидируются на F407VG;
F401CE и F411CE входят в CI-матрицу.

## Тактирование

`SystemCoreClock` настраивается в SDK-овском override `__initialize_hardware()`.
Делители APB1 / APB2 читаются из `RCC->CFGR.PPRE1/PPRE2`. Используйте:

```cpp
import driver.stm32f4.clock;

uint32_t apb1 = driver::stm32f4::getApb1Clock();
uint32_t apb2 = driver::stm32f4::getApb2Clock();
uint32_t timeout = driver::stm32f4::getTimeoutLoops();
```

Не предполагайте `SystemCoreClock / 4` для APB1 — делитель зависит от чипа
и сброса.

## Карта памяти

`sdk/hal/stm32f4/ldscripts/mem.ld.in` заполняется на этапе configure:

- Origin flash (`0x08000000`) и размер — из `STM32_CHIP`.
- Origin RAM (`0x20000000`) и размер.
- Origin CCM (`0x10000000`) и размер — для чипов, где CCM есть
  (F405/407/427/429/437/439).

Linker script — `sdk/core/ldscripts/sections.ld`. `-nostartfiles` обязателен:
без него newlib `crt0` перекроет SDK-овский `_start`.

## Vector table

Vector tables генерируются для каждого семейства из CMSIS-заголовков. Чтобы
обработать прерывание периферии, достаточно определить
`extern "C" void XXX_IRQHandler()` в своём коде; если не определено —
используется weak default из SDK (вызывает `__default_irq_handler()` —
сейчас бесконечный цикл).

## Newlib `nano.specs`

Сборка использует `nano.specs` для уменьшения flash. Побочные эффекты:

- `printf` не линкует `%f` по умолчанию. Либо добавьте
  `target_link_options(... PRIVATE -u _printf_float)`, либо используйте
  целочисленное форматирование.
- `malloc` однокучный, не thread-safe по умолчанию. С FreeRTOS мы используем
  `heap_4` из самого FreeRTOS, а не newlib-овский `_sbrk`.

## Даташиты

Справочные материалы ST — [Даташиты](datasheets.md).
