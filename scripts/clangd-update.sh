#!/usr/bin/env bash
set -euo pipefail

# Prefer the normal dev workflow — no separate configure needed:
#   uv pip install -e .   # or: pip install -e .
# pyproject.toml pins [tool.scikit-build] build-dir = "build" and
# CMAKE_EXPORT_COMPILE_COMMANDS=ON, so the editable build drops
# build/compile_commands.json (same path .clangd uses).
#
# This script is only a fallback when you want compile_commands without
# going through pip (e.g. CI or editor-only checkout).

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="${ROOT_DIR}/build"

cmake -S "${ROOT_DIR}" \
  -B "${BUILD_DIR}" \
  -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
