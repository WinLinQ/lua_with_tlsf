#!/bin/sh
set -eu

ROOT=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
BUILD="$ROOT/build"

git -C "$ROOT" submodule update --init --recursive

rm -rf "$BUILD"
mkdir -p "$BUILD"
cp -a "$ROOT/lua" "$BUILD/lua"
cp -a "$ROOT/tlsf" "$BUILD/tlsf"
cp -a "$ROOT/src" "$BUILD/src"
rm -rf "$BUILD/lua/.git" "$BUILD/tlsf/.git"

(cd "$BUILD/lua" && patch -p1 < "$ROOT/patches/lua-use-tlsf-allocator.patch")
make -C "$BUILD/lua" USE_TLSF=1 "$@"
