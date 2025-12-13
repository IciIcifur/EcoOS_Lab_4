#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"

if [ ! -f "$ECO_FRAMEWORK/Eco.Core1/SharedFiles/IEcoBase1.h" ]; then
  echo "ECO_FRAMEWORK not set correctly: $ECO_FRAMEWORK"
  exit 1
fi

# Build static library
pushd "$PROJECT_ROOT/AssemblyFiles/EcoOS/aarch64_gcc_13_2_1" >/dev/null
./build.sh
popd >/dev/null

# Build and run unit test
pushd "$SCRIPT_DIR" >/dev/null
make
./lab4_unit_test
popd >/dev/null