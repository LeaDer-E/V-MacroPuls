#!/usr/bin/env bash
set -euo pipefail

echo "============================================================================"
echo "  V-MacroPlus v3.9.0 (C++20 / Qt 6) - Linux Build"
echo "============================================================================"

cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j"$(nproc)"

echo ""
echo "Build succeeded! Run with: ./build/V-MacroPlus"
