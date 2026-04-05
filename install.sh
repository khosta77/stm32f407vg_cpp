#!/usr/bin/env bash
set -euo pipefail

REPO="https://github.com/khosta77/stm32-sdk.git"
TOOL_PATH="tools/stmtool"

echo "=== stmtool installer ==="

check_cmd() {
    command -v "$1" >/dev/null 2>&1
}

if ! check_cmd python3; then
    echo "Error: python3 not found. Install Python 3.10+ first."
    exit 1
fi

PY_VERSION=$(python3 -c 'import sys; print(f"{sys.version_info.minor}")')
if [ "$PY_VERSION" -lt 10 ]; then
    echo "Error: Python 3.10+ required, found 3.${PY_VERSION}"
    exit 1
fi

if ! check_cmd pipx; then
    echo "pipx not found, installing..."
    python3 -m pip install --user pipx 2>/dev/null || pip install --user pipx
    python3 -m pipx ensurepath
    export PATH="$HOME/.local/bin:$PATH"
fi

SPEC="stmtool @ git+${REPO}#subdirectory=${TOOL_PATH}"

if check_cmd stmtool; then
    echo "Reinstalling stmtool (latest from git)..."
    pipx install --force "$SPEC"
else
    echo "Installing stmtool..."
    pipx install "$SPEC"
fi

SHELL_NAME=$(basename "$SHELL")
COMPLETION_MARKER="# stmtool completion"

case "$SHELL_NAME" in
    zsh)
        RC_FILE="$HOME/.zshrc"
        ;;
    bash)
        RC_FILE="$HOME/.bashrc"
        ;;
    *)
        RC_FILE=""
        ;;
esac

if [ -n "$RC_FILE" ]; then
    if ! grep -q "$COMPLETION_MARKER" "$RC_FILE" 2>/dev/null; then
        echo ""
        echo "Installing $SHELL_NAME completion..."
        {
            echo ""
            echo "$COMPLETION_MARKER"
            stmtool completion "$SHELL_NAME" 2>/dev/null
        } >> "$RC_FILE"
        echo "Completion added to $RC_FILE"
    fi
fi

echo ""
echo "stmtool installed:"
stmtool version
echo ""
echo "Run 'stmtool doctor' to check your environment."
