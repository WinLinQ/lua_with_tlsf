#!/bin/sh
set -eu

ROOT=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
LUA="$ROOT/build/lua/lua"

if [ ! -x "$LUA" ]; then
  "$ROOT/scripts/build.sh"
fi

"$LUA" -e 'local t={}; for i=1,100000 do t[i]=i end; collectgarbage(); assert(t[100000] == 100000); print("ok")'
