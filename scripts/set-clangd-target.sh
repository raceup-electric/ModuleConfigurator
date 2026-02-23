#!/usr/bin/env sh
set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"

PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"

cd "$PROJECT_ROOT"

INSTANCES_DIR="instances"

if [ ! -d "$INSTANCES_DIR" ]; then
  echo "Error: '$INSTANCES_DIR' directory not found in $PROJECT_ROOT"
  exit 1
fi

if [ -z "$1" ]; then
  echo "Available targets:"
  ls "$INSTANCES_DIR"
  echo
  echo "Usage: $(basename "$0") <target>"
  exit 0
fi

TARGET="$1"
BUILD_DIR=".build-$TARGET"

if [ ! -d "$INSTANCES_DIR/$TARGET" ]; then
  echo "Error: unknown target '$TARGET'"
  echo "Available targets:"
  ls "$INSTANCES_DIR"
  exit 1
fi

if [ ! -d "$BUILD_DIR" ]; then
  echo "Error: build directory '$BUILD_DIR' does not exist"
  echo "Run: cmake --preset $TARGET"
  exit 1
fi

cat > .clangd <<EOF
CompileFlags:
  CompilationDatabase: $BUILD_DIR
EOF

echo "✔ clangd target set to '$TARGET'"
echo "✔ .clangd -> $BUILD_DIR"
