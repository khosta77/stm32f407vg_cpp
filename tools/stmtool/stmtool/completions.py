from __future__ import annotations

import sys
from typing import TYPE_CHECKING

if TYPE_CHECKING:
    pass

_CHIPS = [
    "STM32F401CC", "STM32F401CE", "STM32F401RE",
    "STM32F405RG",
    "STM32F407VE", "STM32F407VG",
    "STM32F411CE", "STM32F411RE",
    "STM32F412VG",
    "STM32F429VI", "STM32F429ZI",
    "STM32F439VI", "STM32F439ZI",
    "STM32F446RE",
]

_FLASH_TOOLS = ["stlink", "openocd", "pyocd", "jlink"]


def complete_chip(incomplete: str) -> list[str]:
    return [c for c in _CHIPS if c.startswith(incomplete.upper())]


def complete_template(incomplete: str) -> list[str]:
    try:
        from stmtool.project import resolve_sdk_root, discover_template
        import sys

        if sys.version_info >= (3, 11):
            import tomllib
        else:
            import tomli as tomllib

        sdk_root = resolve_sdk_root()
        templates_dir = sdk_root / "templates"
        names: list[str] = []
        for cat in sorted(templates_dir.iterdir()):
            if not cat.is_dir():
                continue
            for tpl in sorted(cat.iterdir()):
                meta = tpl / "template.toml"
                if not meta.exists():
                    continue
                with open(meta, "rb") as f:
                    data = tomllib.load(f)
                name = data.get("template", {}).get("name", "")
                if name and name.startswith(incomplete):
                    names.append(name)
        return names
    except Exception:
        return ["blink"]


def complete_flash_tool(incomplete: str) -> list[str]:
    return [t for t in _FLASH_TOOLS if t.startswith(incomplete)]
