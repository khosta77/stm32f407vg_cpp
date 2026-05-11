# `stmtool` reference

`stmtool` is the Python CLI that scaffolds projects, builds firmware, and
manages the SDK cache. It is distributed as a Python package under
`tools/stmtool/` and installed either via `./install.sh` (recommended) or
`pip install ./tools/stmtool`.

## Global options

| Variable | Description |
|----------|-------------|
| `STMSDK_PATH` | Explicit SDK root. Overrides cache lookup. |
| `STMTOOL_LANG` | UI language: `en` (default), `ru`. |

## Commands

### `stmtool project create`

```bash
stmtool project create <name> --chip <chip> [--template <tpl>] [--with-claude]
```

Create a new project directory from a template.

| Flag | Default | Description |
|------|---------|-------------|
| `--chip` | required | STM32 chip in the form `STM32F4xxYZ` (e.g. `STM32F407VG`) |
| `--template` | `blink` | Template name. See `stmtool project templates`. |
| `--with-claude` | off | Also generate a `CLAUDE.md` from the template's `CLAUDE.md.template` |

Generated layout (depends on template):

```
<name>/
  src/main.cpp
  CMakeLists.txt
  stmproject.toml
  CLAUDE.md          # only if --with-claude
  .gitignore
```

The `[sdk] version` field in `stmproject.toml` defaults to `develop`. Edit it
to pin a specific tag.

### `stmtool project templates`

```bash
stmtool project templates
```

List all available templates with their categories and descriptions.

### `stmtool build`

```bash
stmtool build [--native] [--release] [--clean] [--chip <chip>] [--verbose]
```

Build the project in the current directory.

| Flag | Default | Description |
|------|---------|-------------|
| `--native` | off | Use host's `arm-none-eabi-gcc` and `cmake` instead of Docker |
| `--release` | off | Build with `-O2 -DNDEBUG` |
| `--clean` | off | Wipe the build directory first |
| `--chip` | from `stmproject.toml` | Override target chip |
| `--verbose` / `-v` | off | Show full CMake/Ninja output |

The Docker image is `ghcr.io/khosta77/stm32-sdk-build:latest`.

### `stmtool flash`

```bash
stmtool flash [--tool <tool>] [--verify] [--erase]
```

Flash the firmware binary onto the connected board.

| Flag | Default | Description |
|------|---------|-------------|
| `--tool` | `stlink` | Programmer (`stlink`, `jlink`, etc., depending on what's installed) |
| `--verify` | off | Read back and verify after writing |
| `--erase` | off | Mass-erase before writing |

### `stmtool sdk update`

```bash
stmtool sdk update [--version <tag>]
```

Update the cached SDK at `~/.stmtool/stm32-sdk/`.

- Without `--version`, uses `[sdk] version` from `stmproject.toml` in the
  current directory if present, otherwise `develop`.
- With `--version 0.1.2`, fetches the `v0.1.2` tag.
- With `--version develop`, pulls the latest from `origin/develop`.

### `stmtool sdk list-versions`

```bash
stmtool sdk list-versions
```

Print all available SDK tags (sorted, newest first). The currently checked-out
version is marked with a trailing `*`.

### `stmtool sdk path`

```bash
stmtool sdk path
```

Print the resolved SDK root path — useful for debugging or scripting.

### `stmtool doctor`

```bash
stmtool doctor
```

Check that all required tools are installed and on `PATH`: Docker (optional),
`arm-none-eabi-gcc`, `cmake`, `st-flash`.

### `stmtool completion`

```bash
stmtool completion <shell>
```

Print a shell-completion script for `bash`, `zsh`, or `fish`. The `install.sh`
script installs this automatically.

### `stmtool version`

```bash
stmtool version
```

Print the `stmtool` version. The version is derived from git tags via
`setuptools-scm`, so it matches the SDK release.

## SDK resolution order

When any command needs the SDK root, `stmtool` looks in this order:

1. `STMSDK_PATH` environment variable.
2. A parent directory of the running `stmtool` executable that contains
   `sdk/` and `templates/` (in-source checkouts).
3. `~/.stmtool/stm32-sdk/` cache — created on first use by `git clone` of
   the repository.

If the cache exists, `stmtool` calls `_checkout_version()` for whichever
version is requested.
