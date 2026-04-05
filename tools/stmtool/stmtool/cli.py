import shutil
import subprocess
from pathlib import Path

import typer
from rich.console import Console
from rich.table import Table

from stmtool import __version__
from stmtool.config import load_config
from stmtool.i18n import t
from stmtool.project import create_project, resolve_sdk_root

app = typer.Typer(name="stmtool", help=t("app_help"), no_args_is_help=True)
project_app = typer.Typer(help=t("project_help"))
app.add_typer(project_app, name="project")

console = Console()

DOCKER_IMAGE = "ghcr.io/khosta77/stm32-sdk-build:latest"


@project_app.command("create", help=t("project_create_help"))
def project_create(
    name: str = typer.Argument(..., help=t("project_name_help")),
    chip: str = typer.Option(..., "--chip", help=t("chip_help")),
    template: str = typer.Option("blink", "--template", help=t("template_help")),
) -> None:
    try:
        with console.status(t("initializing_git")):
            path = create_project(name, chip, template)
        console.print(f"[bold green]{t('project_created', name=name, path=path)}[/bold green]")
    except (RuntimeError, ValueError, FileExistsError) as e:
        console.print(f"[red]{e}[/red]")
        raise typer.Exit(code=1)


@app.command(help=t("build_help"))
def build(
    release: bool = typer.Option(False, "--release", help=t("build_release")),
    native: bool = typer.Option(False, "--native", help=t("build_native")),
    verbose: bool = typer.Option(False, "--verbose", "-v", help=t("build_verbose")),
    chip: str = typer.Option(None, "--chip", help=t("build_chip")),
    clean: bool = typer.Option(False, "--clean", help=t("build_clean")),
) -> None:
    config = {}
    config_path = Path("stmproject.toml")
    if config_path.exists():
        config = load_config(config_path)

    if clean:
        console.print(f"[yellow]{t('cleaning')}[/yellow]")
        shutil.rmtree("build", ignore_errors=True)

    try:
        sdk_root = resolve_sdk_root()
        sdk_path = sdk_root / "sdk"
    except RuntimeError as e:
        console.print(f"[red]{e}[/red]")
        raise typer.Exit(code=1)

    target_chip = chip or config.get("target", {}).get("chip")
    if not target_chip:
        console.print(f"[red]{t('no_chip')}[/red]")
        raise typer.Exit(code=1)

    build_type = "Release" if release else "Debug"
    verbose_flag = "--verbose" if verbose else ""

    if native:
        cmd = f"cmake -B build -DSTM32_CHIP={target_chip} -DSTM32_SDK={sdk_path} -DCMAKE_BUILD_TYPE={build_type} && cmake --build build {verbose_flag}"
        mode = t("mode_local")
        console.print(f"[bold green]{t('building', chip=target_chip, build_type=build_type, mode=mode)}[/bold green]")
        result = subprocess.run(cmd, shell=True)
    else:
        cmake_cmd = f"cmake -B build -DSTM32_CHIP={target_chip} -DSTM32_SDK=/sdk-repo/sdk -DCMAKE_BUILD_TYPE={build_type} && cmake --build build {verbose_flag}"
        cmd = [
            "docker", "run", "--rm",
            "-v", f"{Path.cwd()}:/workspace",
            "-v", f"{sdk_root}:/sdk-repo:ro",
            "-w", "/workspace",
            DOCKER_IMAGE,
            "bash", "-c", cmake_cmd,
        ]
        mode = t("mode_docker")
        console.print(f"[bold green]{t('building', chip=target_chip, build_type=build_type, mode=mode)}[/bold green]")
        result = subprocess.run(cmd)

    raise typer.Exit(code=result.returncode)


@app.command(help=t("flash_help"))
def flash(
    tool: str = typer.Option(None, "--tool", help=t("flash_tool")),
    verify: bool = typer.Option(False, "--verify", help=t("flash_verify")),
    erase: bool = typer.Option(False, "--erase", help=t("flash_erase")),
) -> None:
    config = {}
    config_path = Path("stmproject.toml")
    if config_path.exists():
        config = load_config(config_path)

    flash_tool = tool or config.get("flash", {}).get("tool", "stlink")

    bin_files = list(Path("build").glob("*.bin"))
    if not bin_files:
        console.print(f"[red]{t('no_bin')}[/red]")
        raise typer.Exit(code=1)

    bin_path = bin_files[0]

    if flash_tool == "stlink":
        cmd = ["st-flash"]
        if erase:
            subprocess.run(["st-flash", "erase"])
        cmd.extend(["--reset", "write", str(bin_path), "0x08000000"])
        if verify:
            cmd.append("--verify")
        console.print(f"[bold green]{t('flashing', name=bin_path.name)}[/bold green]")
        result = subprocess.run(cmd)
        raise typer.Exit(code=result.returncode)
    else:
        console.print(f"[yellow]{t('not_implemented')}[/yellow]")
        raise typer.Exit(code=1)


@app.command(help=t("doctor_help"))
def doctor() -> None:
    table = Table(title="stmtool doctor")
    table.add_column("Component", style="bold")
    table.add_column("Status")
    table.add_column("Details")

    checks = [
        ("Docker", ["docker", "--version"]),
        ("arm-none-eabi-gcc", ["arm-none-eabi-gcc", "--version"]),
        ("CMake", ["cmake", "--version"]),
        ("st-flash", ["st-flash", "--version"]),
    ]

    for name, cmd in checks:
        try:
            result = subprocess.run(cmd, capture_output=True, text=True, timeout=5)
            version_line = result.stdout.split("\n")[0].strip() if result.returncode == 0 else result.stderr.split("\n")[0].strip()
            if result.returncode == 0:
                table.add_row(name, "[green]OK[/green]", version_line)
            else:
                table.add_row(name, "[red]ERROR[/red]", version_line)
        except FileNotFoundError:
            table.add_row(name, "[red]NOT FOUND[/red]", f"Install {name}")
        except subprocess.TimeoutExpired:
            table.add_row(name, "[yellow]TIMEOUT[/yellow]", "")

    console.print(table)


@app.command(help=t("version_help"))
def version() -> None:
    console.print(f"stmtool {__version__}")


if __name__ == "__main__":
    app()
