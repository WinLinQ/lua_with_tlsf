# Lua with TLSF allocator

This repository builds Lua 5.5 with an optional TLSF-backed allocator. The TLSF
pool is allocated with `mmap` and `MAP_POPULATE` to avoid soft page faults during
normal execution.

## Layout

- `lua/`: Lua upstream submodule, pinned to `v5.5.0`
- `tlsf/`: TLSF upstream submodule
- `src/`: allocator implementation owned by this repository
- `patches/`: small integration patch applied to a build copy of Lua
- `scripts/`: build and smoke-test helpers

## Build

Clone with submodules:

```sh
git clone --recurse-submodules <repo-url>
cd lua_with_tlsf
```

Build the TLSF variant:

```sh
scripts/build.sh
```

Run a smoke test:

```sh
scripts/smoke-test.sh
```

Generate `compile_commands.json` for clangd or other editor tooling:

```sh
scripts/gen-compile-commands.sh
```

The build happens under `build/`, so the `lua/` and `tlsf/` submodules stay
clean.

## Allocator design

This project keeps Lua and TLSF as upstream submodules. The allocator code lives
outside the Lua tree in `src/`, and the small Lua integration is stored as
`patches/lua-use-tlsf-allocator.patch`.

The TLSF allocator is used only when Lua is built with `USE_TLSF=1`. It creates a
single fixed-size pool with `mmap` and `MAP_POPULATE`, then routes Lua allocation
requests through `tlsf_realloc` and `tlsf_free`.

The allocator is intentionally simple for measurement:

- one process-wide TLSF pool
- no fallback to libc allocation
- pool exhaustion reports allocation failure to Lua
- no internal locking around TLSF state

## Pool size

The default TLSF pool is 128 MiB. Override it with `MYCFLAGS`:

```sh
scripts/build.sh MYCFLAGS='-DLUA_TLSF_POOL_SIZE=((size_t)1024*1024*1024)'
```

## Upstream maintenance

Lua is tracked as a submodule at tag `v5.5.0`. To update Lua later, move the
submodule to the desired upstream tag and refresh
`patches/lua-use-tlsf-allocator.patch` if Lua's `lauxlib.c` or `makefile`
changed.
