#!/usr/bin/env sh
set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"

cd "$PROJECT_ROOT"

rm -rf \
  .build-* \
  build \
  cmake-build-* \
  CMakeCache.txt \
  CMakeFiles \
  compile_commands.json \
  .clangd

echo "Clean done"

