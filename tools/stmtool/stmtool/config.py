from pathlib import Path
import sys

if sys.version_info >= (3, 11):
    import tomllib
else:
    import tomli as tomllib


def load_config(path: Path = Path("stmproject.toml")) -> dict:
    with open(path, "rb") as f:
        return tomllib.load(f)
