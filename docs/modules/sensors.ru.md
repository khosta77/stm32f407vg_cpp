# Интерфейсы сенсоров

Драйверы сенсоров живут в `sdk/sensors/`. Идея: каждый сенсор данного типа
реализует один и тот же абстрактный интерфейс — код приложения не меняется
при замене SSD1306 на другой I2C-дисплей.

## `IDisplay` — `sensor.display`

```cpp
import sensor.display;
using sensor::IDisplay;

class MyDisplay : public IDisplay {
public:
    driver::Status init() override;
    void clear() override;
    void setPixel(uint16_t x, uint16_t y, bool on) override;
    void drawChar(uint16_t x, uint16_t y, char c) override;
    void drawText(uint16_t x, uint16_t y, const char *text) override;
    driver::Status flush() override;
    uint16_t width() const override;
    uint16_t height() const override;
};
```

`drawText` намеренно принимает `const char *` (не `std::string_view`) — чтобы
call-сайтам не приходилось `#include <string_view>`. Это ограничение C++20
модулей в этом SDK: header-include в `main.cpp` могут конфликтовать с
`import` модуля, использующего тот же header в global module fragment.

Существующая реализация: `sensor.ssd1306`.

## `IExternalFlash` — `sensor.external_flash`

```cpp
import sensor.external_flash;
using sensor::IExternalFlash;

class MyFlash : public IExternalFlash {
public:
    driver::Status init() override;
    driver::Status read(uint32_t addr, std::span<uint8_t> data) override;
    driver::Status writePage(uint32_t addr, std::span<const uint8_t> data) override;
    driver::Status eraseSector(uint32_t addr) override;
    driver::Status chipErase() override;
    uint32_t capacity() const override;
    uint32_t sectorSize() const override;
    uint32_t pageSize() const override;
    uint32_t jedecId() override;
};
```

Запись — пагинированная, стирание — посекторное. `chipErase()` долгий (секунды),
по возможности используйте посекторное стирание.

Существующая реализация: `sensor.w25q32`.

## MPU6050

У MPU6050 пока нет общего IMU-интерфейса (это единственный поставляемый IMU).
Драйвер использует простой `Sample`-struct:

```cpp
struct Sample {
    int16_t ax, ay, az;
    int16_t temp;
    int16_t gx, gy, gz;
};
```

Общий IMU-интерфейс появится по тому же шаблону, что `IDisplay` /
`IExternalFlash`, когда добавится второй IMU-драйвер.

## Добавление нового сенсора

1. Выберите интерфейс — если для типа сенсора он уже есть, реализуйте его;
   иначе создайте `sensor/<тип>.cppm` с virtual-интерфейсом.
2. Создайте `sdk/sensors/<категория>/<имя>/<имя>.cppm` (например,
   `sdk/sensors/displays/ssd1306/ssd1306.cppm`).
3. Подключите модуль в `sdk/cmake/stm32_sensors.cmake`.
4. Добавьте ссылки на даташит и распиновку в `docs/sensors/<имя>.md`
   (EN и RU).
5. Обновите `docs/modules/sensors.md`, если интерфейс поменялся.
