#!/usr/bin/env bash
# ==============================================================================
# One-Click Linux Standalone Build Script for V-MacroPlus (Python Edition v3.2.0)
# Runs automated unit tests, generates multi-size 3D Metallic V+ icons, and
# builds the standalone Linux binary in dist/V-MacroPlus/V-MacroPlus
# ==============================================================================
set -e
cd "$(dirname "$0")"

PYTHON_BIN="python3"
if [ -x ".venv/bin/python" ]; then
    PYTHON_BIN=".venv/bin/python"
fi

echo "=== [1/3] Running Automated Unit Test Suite (21 Tests) ==="
"$PYTHON_BIN" -m unittest discover -s tests -v

echo "=== [2/3] Building Standalone Linux Application via build_exe.py ==="
"$PYTHON_BIN" build_exe.py "$@"

echo "=== [3/3] Linux Build Complete! Binary is in dist/V-MacroPlus/V-MacroPlus ==="
