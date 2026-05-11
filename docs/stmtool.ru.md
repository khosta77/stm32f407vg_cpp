# Справочник `stmtool`

`stmtool` — Python-CLI для создания проектов, сборки прошивок и управления
кэшем SDK. Распространяется как Python-пакет в `tools/stmtool/`,
устанавливается через `./install.sh` (рекомендуется) или
`pip install ./tools/stmtool`.

## Глобальные настройки

| Переменная | Описание |
|------------|----------|
| `STMSDK_PATH` | Явный путь к корню SDK. Перебивает поиск в кэше. |
| `STMTOOL_LANG` | Язык интерфейса: `en` (по умолчанию), `ru`. |

## Команды

### `stmtool project create`

```bash
stmtool project create <имя> --chip <чип> [--template <tpl>] [--with-claude]
```

Создать новый проект из шаблона.

| Флаг | По умолчанию | Описание |
|------|--------------|----------|
| `--chip` | обязательно | Чип STM32 в формате `STM32F4xxYZ` (например `STM32F407VG`) |
| `--template` | `blink` | Имя шаблона. См. `stmtool project templates`. |
| `--with-claude` | выкл. | Сгенерировать `CLAUDE.md` из `CLAUDE.md.template` шаблона |

Структура созданного проекта (зависит от шаблона):

```
<имя>/
  src/main.cpp
  CMakeLists.txt
  stmproject.toml
  CLAUDE.md          # только при --with-claude
  .gitignore
```

Поле `[sdk] version` в `stmproject.toml` по умолчанию `develop`. Отредактируйте,
чтобы зафиксировать конкретный тег.

### `stmtool project templates`

```bash
stmtool project templates
```

Показать все доступные шаблоны с категориями и описанием.

### `stmtool build`

```bash
stmtool build [--native] [--release] [--clean] [--chip <чип>] [--verbose]
```

Собрать проект в текущей директории.

| Флаг | По умолчанию | Описание |
|------|--------------|----------|
| `--native` | выкл. | Использовать хостовые `arm-none-eabi-gcc` и `cmake` вместо Docker |
| `--release` | выкл. | Сборка с `-O2 -DNDEBUG` |
| `--clean` | выкл. | Очистить директорию сборки перед сборкой |
| `--chip` | из `stmproject.toml` | Переопределить целевой чип |
| `--verbose` / `-v` | выкл. | Показывать полный вывод CMake/Ninja |

Docker-образ — `ghcr.io/khosta77/stm32-sdk-build:latest`.

### `stmtool flash`

```bash
stmtool flash [--tool <tool>] [--verify] [--erase]
```

Прошить бинарник в подключённую плату.

| Флаг | По умолчанию | Описание |
|------|--------------|----------|
| `--tool` | `stlink` | Программатор (`stlink`, `jlink`, …, в зависимости от того, что установлено) |
| `--verify` | выкл. | Прочитать и сверить после записи |
| `--erase` | выкл. | Полное стирание перед записью |

### `stmtool sdk update`

```bash
stmtool sdk update [--version <тег>]
```

Обновить кэш SDK в `~/.stmtool/stm32-sdk/`.

- Без `--version` — использует `[sdk] version` из `stmproject.toml` текущей
  директории, если он есть, иначе `develop`.
- С `--version 0.1.2` — забирает тег `v0.1.2`.
- С `--version develop` — pull последних изменений с `origin/develop`.

### `stmtool sdk list-versions`

```bash
stmtool sdk list-versions
```

Печатает все доступные теги SDK (сортировка по убыванию). Текущая
checked-out версия помечается `*` в конце строки.

### `stmtool sdk path`

```bash
stmtool sdk path
```

Печатает резолвнутый путь к корню SDK — удобно для отладки и скриптов.

### `stmtool doctor`

```bash
stmtool doctor
```

Проверяет наличие в `PATH`: Docker (опционально), `arm-none-eabi-gcc`,
`cmake`, `st-flash`.

### `stmtool completion`

```bash
stmtool completion <shell>
```

Печатает скрипт shell-completion для `bash`, `zsh` или `fish`. `install.sh`
устанавливает его автоматически.

### `stmtool version`

```bash
stmtool version
```

Печатает версию `stmtool`. Версия выводится из git-тегов через
`setuptools-scm`, поэтому совпадает с релизом SDK.

## Порядок поиска SDK

Когда команде нужен корень SDK, `stmtool` проверяет в порядке:

1. Переменную окружения `STMSDK_PATH`.
2. Родительские директории исполняемого `stmtool`, содержащие `sdk/` и
   `templates/` (для checkout-сборок).
3. Кэш `~/.stmtool/stm32-sdk/` — создаётся первой командой через
   `git clone` репозитория.

Если кэш существует, `stmtool` вызывает `_checkout_version()` для нужной версии.
