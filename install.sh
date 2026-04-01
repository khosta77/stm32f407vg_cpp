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

if check_cmd stmtool; then
    echo "Upgrading stmtool..."
    pipx upgrade stmtool 2>/dev/null || pipx install --force "stmtool @ git+${REPO}#subdirectory=${TOOL_PATH}"
else
    echo "Installing stmtool..."
    pipx install "stmtool @ git+${REPO}#subdirectory=${TOOL_PATH}"
fi

echo ""
echo "stmtool installed:"
stmtool version
echo ""
echo "Run 'stmtool doctor' to check your environment."
