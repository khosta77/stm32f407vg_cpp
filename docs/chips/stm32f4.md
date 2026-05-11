# STM32F4 family

The reference target for the SDK. All drivers and templates are validated on
F407VG; F401CE and F411CE are part of the CI matrix.

## Clocks

`SystemCoreClock` is set up by the SDK's `__initialize_hardware()` override.
APB1 / APB2 clock divisors are read from `RCC->CFGR.PPRE1/PPRE2`. Use:

```cpp
import driver.stm32f4.clock;

uint32_t apb1 = driver::stm32f4::getApb1Clock();
uint32_t apb2 = driver::stm32f4::getApb2Clock();
uint32_t timeout = driver::stm32f4::getTimeoutLoops();
```

Do **not** assume `SystemCoreClock / 4` for APB1 — the divisor depends on the
chip and reset configuration.

## Memory layout

`sdk/hal/stm32f4/ldscripts/mem.ld.in` is filled at configure time with:

- Flash origin (`0x08000000`) and size from `STM32_CHIP`.
- RAM origin (`0x20000000`) and size.
- CCM origin (`0x10000000`) and size for chips that have it (F405/407/427/429/437/439).

The linker script lives in `sdk/core/ldscripts/sections.ld`. `-nostartfiles` is
mandatory — without it newlib's `crt0` overrides the SDK `_start`.

## Vector table

Vector tables are generated per-family from CMSIS headers. Implementing a
peripheral interrupt is a matter of defining `extern "C" void XXX_IRQHandler()`
in your code; if you don't define it, the weak default in the SDK is used
(which calls `__default_irq_handler()` — currently an infinite loop).

## Newlib `nano.specs`

The build uses `nano.specs` to keep flash usage low. Side effects:

- `printf` doesn't link `%f` by default. Either add
  `target_link_options(... PRIVATE -u _printf_float)` or use integer
  formatting.
- `malloc` is single-heap, not thread-safe by default. With FreeRTOS we use
  `heap_4` from FreeRTOS itself rather than newlib's `_sbrk`.

## Datasheets

See [Datasheets](datasheets.md) for ST's reference material.
