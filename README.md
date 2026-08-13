# Light Helper

<p align="center">
  <img src="docs/assets/logo.png" alt="lh — Light Helper" height="256"/>
  <br/>
  <img src="https://img.shields.io/badge/Compatible%20with-Windows%20Vista-1B6FDE?style=for-the-badge&logo=windows&logoColor=white" alt="Compatible with Windows Vista"/>
</p>

A lightweight C utility library (headers + a small shared/static library) with portable numeric and character types, compiler portability macros, safe cast helpers, **structured errors** (`lh_error_t`), and an optional **setjmp/longjmp-style runtime exception path** built on that error model.

## Features

- **Portable types** — fixed-width integers (`lh_uint_t`, `lh_sllong_t`, …), explicit `lh_uchar_t` / `lh_schar_t`, `lh_bool_t`, `lh_void` / `lh_void_ptr`
- **Compiler layer** — detection (Clang / GCC / MSVC), version, target OS, C++ vs C, `extern "C"` via `LH_COMPILER(EXTERN_C_BEGIN|END)`, attributes (`LH_ATTRIBUTE(SYMBOL)`, …), **subsystem detection macros** (`lh/compiler/os/subsys.h`) for Windows-specific visibility attributes
- **Casts** — `lh_cast_static`, `lh_cast_reinterpret` (C++-style in C++, C casts in C)
- **Pointers** — `lh_ptr`, `lh_void_ptr`, `lh_ptr_of`, `lh_cptr_of`, `lh_str_ptr` / `lh_str_cptr` (`lh/ptr.h`, `lh/void/ptr.h`, `lh/util/ptr.h`, `lh/str/ptr.h`)
- **Compound literals** — `lh_initializer`, `lh_initializer_of_type`, … in `lh/initializer.h`
- **Member-offset utility** — `lh_offset_of` macro in `lh/offset.h` for compile-time layout checks
- **Macro utilities** — argument token concatenation (`lh_arg_concat`) and count (`lh_arg_get_count`) for variadic macro dispatch in `lh/util/arg.h`
- **Runtime assertions** — `lh_assert_runtime`, `lh_assert_runtime_if`, `lh_assert_runtime_ifn`, `lh_assert_runtime_ref` — conditional throw with auto-detecting forms (code, message, or both) in `lh/assert.h`
- **Errors** — `lh_error_t` (code + description); description type is `lh_error_desc_t` in `lh/error/desc.h` (today an alias of `lh_str_cptr`)
- **Exceptions (runtime)** — `lh_exception_t` wrapping `lh_error_t`, optional debug `origin`, catch stack and throw macros under `lh/runtime/`, raise dispatch (`lh_runtime_raise`) with smart argument type detection
- **Memory & intervals** — non-owning memory views (`lh_memory_view_t`) and bounds (`lh_memory_bounds_t`) with front/back and indexed access, validated and fallback APIs (`*_v`, `*_or_empty`); read-only closed-byte-slice type `lh_memory_view_slice_t` (`lh/memory/view/slice.h`) with flags, direction, size, containment, overlap, alignment, and indexed access helpers; numeric interval helpers and binary-search macros used by table lookups
- **Memory allocator** — configurable alloc/dealloc callback pair (`lh_memory_allocator_t`, `lh/memory/allocator.h`) with pack/unpack/init/set and validated alloc/dealloc/realloc; a thread-local runtime singleton (`lh_runtime_allocator()`, `lh/runtime/allocator.h`) seeded at compile time from a configurable default pair (`malloc`/`free` unless overridden — see Library options below)
- **Heap-owning memory bounds** — `lh_memory_bounds_allocated_t` (`lh/memory/bounds/allocated.h`) is layout-identical to `lh_memory_bounds_t` but routes `clear` / `resize` / `exchange` through the runtime allocator, freeing or reallocating the owned block as needed
- **Wide text** — `lh_wchar_t`, raw wide-string helpers, Unicode simple case mapping for buffers and single-code-point case fold (UCD-backed tables under `src/lh/util/wstr/` and `src/lh/util/wchar/`)
- **String search & compare** — `lh_str_view_find`, `lh_str_view_rfind`, `lh_str_view_find_of`, case-insensitive compare, size-aware comparison; same for wide strings (`lh_wstr_ptr_*`)
- **Version** — `lh_version_t`, `lh_get_version()` via `lh/lh.h`
- **Build** — CMake, generated `lh/config.h`, optional Doxygen docs and bundled GoogleTest

## Requirements

| Tool        | Minimum version |
|-------------|-----------------|
| CMake       | 3.12            |
| C compiler  | **C11** or later |
| Doxygen     | any (optional, for docs) |
| Graphviz    | any (optional, for graphs) |
| GoogleTest  | bundled via submodule (optional, for tests) |

## Building

```sh
cmake -S . -B build
cmake --build build
```

Convenience scripts are available for common platform builds:

```powershell
.\scripts\build-windows.ps1 -Config Release -RunTests
```

If local PowerShell policy blocks unsigned scripts, run the same build with:

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File .\scripts\build-windows.ps1 -Config Release -RunTests
```

```sh
./scripts/build-linux.sh --config Release --run-tests
```

Both scripts accept options for static/shared builds, docs/tests, target, build
directory, compiler selection, clean builds, and parallelism. See
[scripts/README.md](scripts/README.md) for the quick command list.

Legacy Windows builds that must work from `cmd.exe` can use the batch wrappers:

```bat
scripts\build-windows-xp.bat --clean
scripts\build-windows-vista.bat --clean
```

Details are in [scripts/README.md](scripts/README.md).

### Options

| CMake option     | Default | Description                          |
|------------------|---------|--------------------------------------|
| `LH_BUILD_SHARED` | `ON`   | Build as a shared library            |
| `LH_BUILD_DOCS`   | `ON`   | Generate Doxygen documentation       |
| `LH_BUILD_TESTS`  | `ON`   | Build GoogleTest targets             |
| `LH_DOCS_GRAPHS`  | `ON`   | Include Graphviz dependency graphs   |

Example — static library without docs:

```sh
cmake -S . -B build -DLH_BUILD_SHARED=OFF -DLH_BUILD_DOCS=OFF
cmake --build build
```

### Library options

These control runtime *behavior* baked into the generated `lh/config.h` (see
`cmake/library_options.cmake`) — distinct from the build-shape options above.
Manual (no-CMake) builds set the same names directly in `config.h` or via `-D`.

| Option | Default | Description |
|---|---|---|
| `LH_LIBRARY_OPTION_THREAD_LOCAL` | `ON` | `LH_ATTRIBUTE_THREAD_LOCAL` uses real TLS in library sources |
| `LH_LIBRARY_OPTION_RUNTIME_TERMINATE_USE_STDLIB` | `ON` | Default `lh_runtime_terminate` handler is `abort()`; `OFF` leaves it unset |
| `LH_LIBRARY_OPTION_MEMORY_ALLOCATOR_USE_STDLIB` | `ON` | Seed the runtime allocator with the default pair below at startup; `OFF` leaves it null until `lh_memory_allocator_set()` is called |
| `LH_LIBRARY_OPTION_MEMORY_ALLOCATOR_DEFAULT_ALLOC` | `malloc` | Function bound as the runtime allocator's `alloc_cb` (e.g. `pvPortMalloc`) |
| `LH_LIBRARY_OPTION_MEMORY_ALLOCATOR_DEFAULT_DEALLOC` | `free` | Function bound as the runtime allocator's `dealloc_cb` (e.g. `vPortFree`) |
| `LH_LIBRARY_OPTION_MEMORY_ALLOCATOR_DEFAULT_INCLUDE` | `<stdlib.h>` | Header declaring the two functions above |
| `LH_LIBRARY_OPTION_MEMORY_ALLOCATOR_INIT_ALLOCATED` | `ON` | Zero-initialize memory returned by `lh_memory_allocator_alloc` |
| `LH_LIBRARY_OPTION_RUNTIME_EXCEPTION_CATCH_STACK_MAX` | `16` | Maximum depth of nested runtime exception catch frames (must be `> 0`) |

Example — bind the runtime allocator to a custom pair at compile time instead
of writing `lh_memory_allocator_set()` init code:

```sh
cmake -S . -B build \
  -DLH_LIBRARY_OPTION_MEMORY_ALLOCATOR_DEFAULT_ALLOC=pvPortMalloc \
  -DLH_LIBRARY_OPTION_MEMORY_ALLOCATOR_DEFAULT_DEALLOC=vPortFree \
  -DLH_LIBRARY_OPTION_MEMORY_ALLOCATOR_DEFAULT_INCLUDE=<FreeRTOS.h>
```

### Generating documentation

```sh
cmake --build build --target lh_docs
```

The HTML output is written to `docs/html/index.html`.

## Usage

Umbrella header (version entry point):

```c
#include <lh/lh.h>

const lh_version_t *v = lh_get_version();
```

Include only what you need, for example:

```c
#include <lh/numeric/types.h>      /* lh_uint_t, lh_sint_t, … */
#include <lh/compiler/extern/c.h>  /* LH_COMPILER_EXTERN_C_BEGIN/END, … */
#include <lh/cast/static.h>        /* lh_cast_static */
#include <lh/cast/reinterpret.h>   /* lh_cast_reinterpret */
#include <lh/util/ptr.h>           /* lh_ptr_of, lh_cptr_of */
#include <lh/initializer.h>        /* lh_initializer_of_type, … */
#include <lh/error.h>              /* lh_error_t, lh_error_pack, … */
#include <lh/str/ptr.h>            /* lh_str_ptr, lh_str_cptr */
```

There is no single umbrella `lh/compiler.h` or `lh/cast.h` — compiler
detection and casts are split into focused headers under `lh/compiler/` and
`lh/cast/`; include only the ones you need.

String-related headers live under **`lh/str/`** (not `lh/string/`). Generic initializer macros are **`lh/initializer.h`** at the include root. `lh_str_view_t` search/compare API is in `lh/str/view.h`; raw buffer helpers are in `lh/util/str/ptr.h`.

### Integer types

```c
lh_uint_t   counter = 0;    /* 32-bit unsigned */
lh_sllong_t offset  = -1;   /* 64-bit signed   */
lh_uchar_t  byte    = 0xFF; /* 8-bit unsigned  */
```

### Version struct

```c
lh_version_t ver = lh_version_initializer(0, 3, 0);
lh_version_major_t major = lh_version_get_major(&ver); /* 0 */
```

## Safety conventions

Most public APIs in this library perform runtime validation and raise an error
on invalid input. However, a small number of functions are intentionally
**unsafe** — they skip range validation to let callers who have already
verified invariants avoid redundant checks. These functions are explicitly
marked with a `@warning` tag in their documentation.

Current unsafe functions:

| Function | What is skipped |
|---|---|
| `lh_memory_bounds_slice_set` | range check on `[begin, end]` |
| `lh_memory_bounds_slice_assign` | range check on the source slice |
| `lh_memory_bounds_slice_make` | range check on `[begin, end]` |
| `lh_memory_bounds_slice_swap` | range check on both operands |

Null-pointer checks are still enforced even in unsafe functions.

**When calling an unsafe function the caller assumes full responsibility** for
the validity of the range. Passing a backward or otherwise invalid range
produces a logically inconsistent slice and may cause failures in any
subsequent operation that requires a valid slice.

## Version

Current version: **0.3.0** — history and release notes in [CHANGELOG](CHANGELOG.md).

Documentation for tools is in [tools/README.md](tools/README.md).

## License

See [LICENSE](LICENSE.md) for details.
