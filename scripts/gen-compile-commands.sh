#!/bin/sh
set -eu

ROOT=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)

cd "$ROOT"
bear --output "$ROOT/compile_commands.json" -- "$ROOT/scripts/build.sh" "$@"
