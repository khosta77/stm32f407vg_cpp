import subprocess
from pathlib import Path

import typer
from rich.console import Console
from rich.table import Table

from stmtool import __version__
from stmtool.config import load_config

app = typer.Typer(name="stmtool", help="STM32-SDK project tool", no_args_is_help=True)
project_app = typer.Typer(help="Project management")
app.add_typer(project_app, name="project")

console = Console()

DOCKER_IMAGE = "ghcr.io/khosta77/stm32-sdk-build:latest"


@project_app.command("create")
def project_create(
    name: str = typer.Argument(..., help="Project name"),
    chip: str = typer.Option(..., "--chip", help="Target STM32 chip (e.g. STM32F407VG)"),
    template: str = typer.Option("blink", "--template", help="Template name"),
) -> None:
    console.print(f"[bold]stmtool project create[/bold] {name} --chip {chip} --template {template}")
    console.print("[yellow]Not yet implemented[/yellow]")
    raise typer.Exit(code=1)


@app.command()
def build(
    release: bool = typer.Option(False, "--release", help="Release build"),
    native: bool = typer.Option(False, "--native", help="Build locally without Docker"),
    verbose: bool = typer.Option(False, "--verbose", "-v", help="Verbose CMake output"),
    chip: str = typer.Option(None, "--chip", help="Override chip from stmproject.toml"),
) -> None:
    config = {}
    config_path = Path("stmproject.toml")
    if config_path.exists():
        config = load_config(config_path)

    target_chip = chip or config.get("target", {}).get("chip")
    if not target_chip:
        console.print("[red]Error: no chip specified (use --chip or set target.chip in stmproject.toml)[/red]")
        raise typer.Exit(code=1)

    build_type = "Release" if release else "Debug"
    verbose_flag = "--verbose" if verbose else ""

    if native:
        cmd = f"cmake -B build -DSTM32_CHIP={target_chip} -DCMAKE_BUILD_TYPE={build_type} && cmake --build build {verbose_flag}"
        console.print(f"[bold green]Building {target_chip} ({build_type}) locally...[/bold green]")
        result = subprocess.run(cmd, shell=True)
    else:
        cmake_cmd = f"cmake -B build -DSTM32_CHIP={target_chip} -DCMAKE_BUILD_TYPE={build_type} && cmake --build build {verbose_flag}"
        cmd = [
            "docker", "run", "--rm",
            "-v", f"{Path.cwd()}:/workspace",
            "-w", "/workspace",
            DOCKER_IMAGE,
            "bash", "-c", cmake_cmd,
        ]
        console.print(f"[bold green]Building {target_chip} ({build_type}) in Docker...[/bold green]")
        result = subprocess.run(cmd)

    raise typer.Exit(code=result.returncode)


@app.command()
def flash(
    tool: str = typer.Option(None, "--tool", help="Flash tool: stlink, openocd, pyocd, jlink"),
    verify: bool = typer.Option(False, "--verify", help="Verify after flash"),
    erase: bool = typer.Option(False, "--erase", help="Erase flash before writing"),
) -> None:
    config = {}
    config_path = Path("stmproject.toml")
    if config_path.exists():
        config = load_config(config_path)

    flash_tool = tool or config.get("flash", {}).get("tool", "stlink")

    bin_files = list(Path("build").glob("*.bin"))
    if not bin_files:
        console.print("[red]Error: no .bin file found in build/. Run 'stmtool build' first.[/red]")
        raise typer.Exit(code=1)

    bin_path = bin_files[0]

    if flash_tool == "stlink":
        cmd = ["st-flash"]
        if erase:
            subprocess.run(["st-flash", "erase"])
        cmd.extend(["--reset", "write", str(bin_path), "0x08000000"])
        if verify:
            cmd.append("--verify")
        console.print(f"[bold green]Flashing {bin_path.name} via st-link...[/bold green]")
        result = subprocess.run(cmd)
        raise typer.Exit(code=result.returncode)
    else:
        console.print(f"[yellow]Flash tool '{flash_tool}' not yet implemented[/yellow]")
        raise typer.Exit(code=1)


@app.command()
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
            version = result.stdout.split("\n")[0].strip() if result.returncode == 0 else result.stderr.split("\n")[0].strip()
            if result.returncode == 0:
                table.add_row(name, "[green]OK[/green]", version)
            else:
                table.add_row(name, "[red]ERROR[/red]", version)
        except FileNotFoundError:
            table.add_row(name, "[red]NOT FOUND[/red]", f"Install {name}")
        except subprocess.TimeoutExpired:
            table.add_row(name, "[yellow]TIMEOUT[/yellow]", "")

    console.print(table)


@app.command()
def version() -> None:
    console.print(f"stmtool {__version__}")


if __name__ == "__main__":
    app()
