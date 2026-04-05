import re
import shutil
import subprocess
import sys
from pathlib import Path

if sys.version_info >= (3, 11):
    import tomllib
else:
    import tomli as tomllib

from stmtool.i18n import t

_CHIP_RE = re.compile(r"^STM32[A-Z]\d{3}[A-Z]{2}$")
_DEFAULT_REPO = "https://github.com/khosta77/stm32-sdk.git"

_GITIGNORE = """\
build/
*.o
*.d
*.elf
*.hex
*.bin
*.map
__pycache__/
.DS_Store
"""


_SDK_CACHE_DIR = Path.home() / ".stmtool" / "stm32-sdk"


def _is_sdk_root(p: Path) -> bool:
    return (p / "sdk").is_dir() and (p / "templates").is_dir()


def _clone_sdk_cache() -> Path:
    import sys

    print(t("sdk_cloning"), file=sys.stderr)
    _SDK_CACHE_DIR.parent.mkdir(parents=True, exist_ok=True)
    result = subprocess.run(
        ["git", "clone", "--depth", "1", _DEFAULT_REPO, str(_SDK_CACHE_DIR)],
        capture_output=True,
        text=True,
    )
    if result.returncode != 0:
        raise RuntimeError(t("sdk_not_found"))
    return _SDK_CACHE_DIR


def resolve_sdk_root() -> Path:
    import os

    env_path = os.environ.get("STMSDK_PATH")
    if env_path:
        p = Path(env_path)
        if _is_sdk_root(p):
            return p

    current = Path(__file__).resolve()
    for parent in current.parents:
        if _is_sdk_root(parent):
            return parent

    if _is_sdk_root(_SDK_CACHE_DIR):
        return _SDK_CACHE_DIR

    return _clone_sdk_cache()


def resolve_sdk_repo_url(sdk_root: Path) -> str:
    import os

    env_repo = os.environ.get("STMSDK_REPO")
    if env_repo:
        return env_repo

    try:
        result = subprocess.run(
            ["git", "-C", str(sdk_root), "remote", "get-url", "origin"],
            capture_output=True,
            text=True,
            timeout=5,
        )
        if result.returncode == 0 and result.stdout.strip():
            return result.stdout.strip()
    except (FileNotFoundError, subprocess.TimeoutExpired):
        pass
    return _DEFAULT_REPO


def discover_template(sdk_root: Path, template_name: str) -> Path:
    templates_dir = sdk_root / "templates"
    available: list[str] = []

    for category_dir in sorted(templates_dir.iterdir()):
        if not category_dir.is_dir():
            continue
        for tpl_dir in sorted(category_dir.iterdir()):
            if not tpl_dir.is_dir():
                continue
            meta_path = tpl_dir / "template.toml"
            if not meta_path.exists():
                continue
            with open(meta_path, "rb") as f:
                meta = tomllib.load(f)
            name = meta.get("template", {}).get("name", "")
            available.append(name)
            if name == template_name:
                return tpl_dir

    raise RuntimeError(
        t("template_not_found", name=template_name, available=", ".join(available))
    )


def create_project(name: str, chip: str, template_name: str) -> Path:
    sdk_root = resolve_sdk_root()
    tpl_dir = discover_template(sdk_root, template_name)
    repo_url = resolve_sdk_repo_url(sdk_root)

    chip = chip.upper()
    if not _CHIP_RE.match(chip):
        raise ValueError(t("invalid_chip", chip=chip))

    target = Path.cwd() / name
    if target.exists():
        raise FileExistsError(t("project_exists", name=name))

    target.mkdir()
    (target / "src").mkdir()

    for item in sorted(tpl_dir.iterdir()):
        if item.name == "template.toml":
            continue

        if item.suffix == ".template":
            content = item.read_text()
            content = content.replace("@PROJECT_NAME@", name)
            content = content.replace("@STM32_CHIP@", chip)
            dest = target / item.stem
            dest.write_text(content)
        elif item.name.endswith(".cpp") or item.name.endswith(".c") or item.name.endswith(".h"):
            shutil.copy2(item, target / "src" / item.name)
        else:
            if item.is_dir():
                shutil.copytree(item, target / item.name)
            else:
                shutil.copy2(item, target / item.name)

    (target / ".gitignore").write_text(_GITIGNORE)

    subprocess.run(["git", "init"], cwd=target, capture_output=True)

    submodule_cmd = ["git"]
    if repo_url.startswith("file://"):
        submodule_cmd += ["-c", "protocol.file.allow=always"]
    submodule_cmd += ["submodule", "add", repo_url, "stm32-sdk"]
    subprocess.run(submodule_cmd, cwd=target, capture_output=True)

    return target
