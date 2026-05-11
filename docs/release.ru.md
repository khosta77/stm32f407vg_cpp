# Релизы

## Политика версионирования

Теги используют SemVer (`vMAJOR.MINOR.PATCH`). Проект находится в pre-1.0:
и patch-, и minor-релизы могут включать небольшие изменения, требующие правок
в пользовательском коде. Где именно проходит граница «patch vs minor» — решает
maintainer, пока API не стабилизируется (цель — v1.0).

Практические рекомендации для подключающих SDK проектов:

- Фиксируйте конкретный тег в `stmproject.toml` (`[sdk] version = "0.1.2"`),
  а не `develop`.
- Перед апгрейдом — прочитайте [заметки по апгрейду](migration.md).
- Используйте `stmtool sdk update --version <тег>` для обновления кэша.

## Инструменты

Номер версии выводится из git-тегов через `setuptools-scm`. Нигде нет ручной
константы — именно тег создаёт релиз для пользователей `stmtool`.

`tools/stmtool/pyproject.toml` конфигурирует:

```toml
[tool.setuptools_scm]
root = "../.."
version_file = "stmtool/_version.py"
```

## Процедура релиза

1. `develop` должен быть зелёным на CI (matrix F407VG/F401CE/F411CE).
2. Локально проверить доки: `mkdocs serve`.
3. Повесить тег на merge-коммит в `develop`:
   ```bash
   git tag -a v0.1.3 -m "Release v0.1.3" <коммит>
   git push origin v0.1.3
   ```
4. Создать GitHub Release с заметками из секции «История релизов» этой страницы.
5. Workflow `docs.yml` пересоберёт сайт автоматически — проверить, что
   <https://khosta77.github.io/stm32-sdk/> подтянул свежий контент.

## История релизов

### v0.1.3 (в подготовке)

Главные изменения:

- Новый сайт документации (MkDocs Material), английский и русский.
- Подкоманды `stmtool sdk update`, `sdk list-versions`, `sdk path`.
- Опциональная генерация `CLAUDE.md` per-template через `--with-claude`.
- Документированы все новые сенсоры (MPU6050, SSD1306, W25Q32) и DMA-обёртка.
- Исправлен I2C multi-byte read по RM0090 §27.3.3, корректен на 400 кГц.
- `GpioConfig` переписан как aggregate с валидатором `consteval gpio({...})`.

См. [заметки по апгрейду](migration.md) о необходимых правках в пользовательском коде.

### v0.1.2

- Усиление `stmtool` install (`install.sh` чистит кэш при переустановке).

### v0.1.1

- FreeRTOS-шаблоны (`freertos/blink`, `freertos/mpu6050-uart`) зарегистрированы
  для discovery в `stmtool`.

### v0.1.0

- Первый релиз. Версионирование через `setuptools-scm` для `stmtool` и SDK
  в целом. CI перенесён с `main` на `develop`.

## GitHub Pages

Сайт документации публикуется из ветки `gh-pages` (создаётся workflow
`docs.yml`). Адрес — <https://khosta77.github.io/stm32-sdk/>. После первого
деплоя — включить Pages в Settings → Pages → Source = `gh-pages` / `(root)`.
