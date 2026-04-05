import os

_LANG = os.environ.get("STMTOOL_LANG", "en").lower()[:2]

_STRINGS: dict[str, dict[str, str]] = {
    "app_help": {
        "en": "STM32-SDK project tool",
        "ru": "Инструмент для проектов STM32-SDK",
    },
    "project_help": {
        "en": "Project management",
        "ru": "Управление проектами",
    },
    "project_create_help": {
        "en": "Create a new STM32 project from template",
        "ru": "Создать новый проект STM32 из шаблона",
    },
    "build_help": {
        "en": "Build the project (Docker by default)",
        "ru": "Собрать проект (Docker по умолчанию)",
    },
    "build_release": {
        "en": "Release build",
        "ru": "Релизная сборка",
    },
    "build_native": {
        "en": "Build locally without Docker",
        "ru": "Собрать локально без Docker",
    },
    "build_verbose": {
        "en": "Verbose CMake output",
        "ru": "Подробный вывод CMake",
    },
    "build_chip": {
        "en": "Override chip from stmproject.toml",
        "ru": "Переопределить чип из stmproject.toml",
    },
    "flash_help": {
        "en": "Flash firmware to target board",
        "ru": "Прошить прошивку на плату",
    },
    "flash_tool": {
        "en": "Flash tool: stlink, openocd, pyocd, jlink",
        "ru": "Инструмент прошивки: stlink, openocd, pyocd, jlink",
    },
    "flash_verify": {
        "en": "Verify after flash",
        "ru": "Проверить после прошивки",
    },
    "flash_erase": {
        "en": "Erase flash before writing",
        "ru": "Стереть flash перед записью",
    },
    "doctor_help": {
        "en": "Check development environment",
        "ru": "Проверить окружение разработки",
    },
    "version_help": {
        "en": "Show stmtool version",
        "ru": "Показать версию stmtool",
    },
    "project_name_help": {
        "en": "Project name",
        "ru": "Имя проекта",
    },
    "chip_help": {
        "en": "Target STM32 chip (e.g. STM32F407VG)",
        "ru": "Целевой чип STM32 (напр. STM32F407VG)",
    },
    "template_help": {
        "en": "Template name",
        "ru": "Имя шаблона",
    },
    "not_implemented": {
        "en": "Not yet implemented",
        "ru": "Ещё не реализовано",
    },
    "no_chip": {
        "en": "Error: no chip specified (use --chip or set target.chip in stmproject.toml)",
        "ru": "Ошибка: чип не указан (используйте --chip или задайте target.chip в stmproject.toml)",
    },
    "building": {
        "en": "Building {chip} ({build_type}){mode}...",
        "ru": "Сборка {chip} ({build_type}){mode}...",
    },
    "mode_local": {
        "en": " locally",
        "ru": " локально",
    },
    "mode_docker": {
        "en": " in Docker",
        "ru": " в Docker",
    },
    "flashing": {
        "en": "Flashing {name} via st-link...",
        "ru": "Прошивка {name} через st-link...",
    },
    "no_bin": {
        "en": "Error: no .bin file found in build/. Run 'stmtool build' first.",
        "ru": "Ошибка: файл .bin не найден в build/. Сначала запустите 'stmtool build'.",
    },
    "project_created": {
        "en": "Project '{name}' created at {path}",
        "ru": "Проект '{name}' создан в {path}",
    },
    "project_exists": {
        "en": "Error: directory '{name}' already exists",
        "ru": "Ошибка: директория '{name}' уже существует",
    },
    "sdk_not_found": {
        "en": "Error: SDK not found. Set STMSDK_PATH environment variable.",
        "ru": "Ошибка: SDK не найден. Установите переменную окружения STMSDK_PATH.",
    },
    "template_not_found": {
        "en": "Error: template '{name}' not found. Available: {available}",
        "ru": "Ошибка: шаблон '{name}' не найден. Доступные: {available}",
    },
    "invalid_chip": {
        "en": "Error: invalid chip format '{chip}'",
        "ru": "Ошибка: неверный формат чипа '{chip}'",
    },
    "initializing_git": {
        "en": "Creating project...",
        "ru": "Создание проекта...",
    },
    "build_clean": {
        "en": "Clean build directory before building",
        "ru": "Очистить директорию сборки перед сборкой",
    },
    "cleaning": {
        "en": "Cleaning build directory...",
        "ru": "Очистка директории сборки...",
    },
}


def t(key: str, **kwargs: str) -> str:
    entry = _STRINGS.get(key, {})
    text = entry.get(_LANG, entry.get("en", key))
    if kwargs:
        text = text.format(**kwargs)
    return text
