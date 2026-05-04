# Light Helper

<p align="center">
  <img src="docs/assets/logo.png" alt="lh — Light Helper" height="256"/>
  <br/>
  <img src="https://img.shields.io/badge/Compatible%20with-Windows%20Vista-1B6FDE?style=for-the-badge&logo=windows&logoColor=white" alt="Compatible with Windows Vista"/>
</p>

A lightweight C utility library (headers + a small shared/static library) with portable numeric and character types, compiler portability macros, safe cast helpers, **structured errors** (`lh_error_t`), and an optional **setjmp/longjmp-style runtime exception path** built on that error model.

## Features

- **Portable types** — fixed-width integers (`lh_uint_t`, `lh_sllong_t`, …), explicit `lh_uchar_t` / `lh_schar_t`, `lh_bool_t`, `lh_void` / `lh_void_ptr`
- **Compiler layer** — detection (Clang / GCC / MSVC), version, target OS, C++ vs C, `extern "C"` via `LH_COMPILER(EXTERN_C_BEGIN|END)`, attributes (`LH_ATTRIBUTE(SYMBOL)`, …), **subsystem detection macros** (`lh/subsys.h`) for Windows-specific visibility attributes
- **Casts** — `lh_cast_static`, `lh_cast_reinterpret` (C++-style in C++, C casts in C)
- **Pointers** — `lh_ptr`, `lh_void_ptr`, `lh_ptr_of`, `lh_cptr_of`, `lh_str_ptr` / `lh_str_cptr` (`lh/ptr.h`, `lh/void/ptr.h`, `lh/util/ptr.h`, `lh/str/ptr.h`)
- **Compound literals** — `lh_initializer`, `lh_initializer_of_type`, … in `lh/initializer.h`
- **Member-offset utility** — `lh_offset_of` macro in `lh/offset.h` for compile-time layout checks
- **Errors** — `lh_error_t` (code + description); description type is `lh_error_desc_t` in `lh/error/desc.h` (today an alias of `lh_str_cptr`)
- **Exceptions (runtime)** — `lh_exception_t` wrapping `lh_error_t`, optional debug `origin`, catch stack and throw macros under `lh/runtime/`
- **Memory & intervals** — non-owning memory views/ranges/typed spans with front/back and indexed access, validated and fallback APIs (`*_v`, `*_or_empty`), typed clone/dup helpers and typed slicing (`lh_memory_typed_slice`), numeric interval helpers and binary-search macros used by table lookups
- **Wide text** — `lh_wchar_t`, raw wide-string helpers, Unicode simple case mapping for buffers and single-code-point case fold (UCD-backed tables under `src/lh/util/wstr/` and `src/lh/util/wchar/`)
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
#include <lh/numeric/types.h>   /* lh_uint_t, lh_sint_t, … */
#include <lh/compiler.h>        /* LH_COMPILER_*, … */
#include <lh/cast.h>            /* lh_cast_static, lh_cast_reinterpret */
#include <lh/util/ptr.h>        /* lh_ptr_of, lh_cptr_of */
#include <lh/initializer.h>     /* lh_initializer_of_type, … */
#include <lh/error.h>           /* lh_error_t, lh_error_pack, … */
#include <lh/str/ptr.h>         /* lh_str_ptr, lh_str_cptr */
```

String-related headers live under **`lh/str/`** (not `lh/string/`). Generic initializer macros are **`lh/initializer.h`** at the include root.

### Integer types

```c
lh_uint_t   counter = 0;    /* 32-bit unsigned */
lh_sllong_t offset  = -1;   /* 64-bit signed   */
lh_uchar_t  byte    = 0xFF; /* 8-bit unsigned  */
```

### Version struct

```c
lh_version_t ver = { 0, 2, 0 };
/* ver.major == 0, ver.minor == 2, ver.patch == 0 */
```

## Version

Current version from `VERSION`: **0.2.0** — history and release notes in [CHANGELOG](CHANGELOG.md).

Git tag **`v0.2.0`** marks this release on GitHub. Earlier **`v0.1.0`** remains available for the initial public snapshot.

Documentation for tools is in [tools/README.md](tools/README.md).

## License

See [LICENSE](LICENSE.md) for details.
