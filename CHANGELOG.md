# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

## [0.2.0] - 2026-04-20

### Added

- GitHub Actions CI workflow for Linux (GCC/Clang) and Windows (MSVC) builds with tests.
- CTest registration for the `lh_test` executable (`enable_testing()` + `add_test(...)`).
- GoogleTest coverage for `lh/memory/std` byte helpers (`copy`, `copy_rev`, `rcopy`,
  `move`, `set`, `compare`, `rcompare`) under mirrored test path `test/src/lh/memory/std.cpp`.
- Portable wide character type (`lh_wchar_t`) and related headers; `lh/uchar.h` for Unicode
  scalar-width aliases.
- Non-owning memory **ranges**, **views**, and **string views** with iteration, slicing,
  find/compare/move/set helpers; aligned pointer grid macros (`lh_ptr_align_up_grid`, …).
- Interval arithmetic macros, pair-field terminology for bounds, and **binary search** helpers
  (`lh_interval_ropen_binary_search`, …) used by generated Unicode tables.
- `lh_wchar_case_pair_t` for code-point mapping pairs; **case folding** for a single wide
  character (`lh_wchar_fold_case`, table in `src/lh/util/wchar/case/fold.c`) and **simple
  case mapping** over wide buffers (`lh_wstr_raw_to_lower`, `lh_wstr_raw_to_upper`, table in
  `src/lh/util/wstr/case/map.c`). Regenerate from UCD via `tools/gen_wstr_case_fold.py` and
  `tools/gen_wchar_case_map.py` (defaults write those paths).
- Raw C string helpers: substring, equality, starts/ends with, fill with a repeating pattern.
- Wide-string helpers: content checks, equality, prefix/suffix, fill with wildcard, optional
  case fold for comparisons where applicable.

### Changed

- Moved `ptr.h`, `type.h`, `bit.h`, and `math.h` to `lh/util/` with include guards
  `LH_UTIL_PTR_H`, `LH_UTIL_TYPE_H`, `LH_UTIL_BIT_H`, and `LH_UTIL_MATH_H`.
  Replace `#include <lh/ptr.h>` (and the other three) with `#include <lh/util/…>`.
- Documentation terminology aligned for pointer aliases (`lh_ptr` and `lh_void_ptr`).
- CI now runs tests through `ctest` instead of direct test binary execution.
- Improved English comments in root/test/docs CMake files for clearer build configuration docs.
- Added `LH_ENABLE_RELEASE_MAX_OPT` (default `ON`) to optionally enable aggressive
  `Release`-only optimization flags and best-effort IPO/LTO for the `lh` target.
- IPO/LTO unsupported toolchains now produce a concise status line instead of verbose
  probe output during configure.
- Wide-character **case fold** and **simple case map** sources reorganized under
  `src/lh/util/wchar/` and `src/lh/util/wstr/`; string comparison helpers refactored for
  clearer pointer use; `lh_memory_view` APIs use `lh_ptr` instead of `lh_void` where
  appropriate for readability and consistency.

### Fixed

- `lh_memory_view_init_by_size` now avoids non-portable `void*` arithmetic (MSVC-compatible).
- Runtime terminate path is hardened with a guaranteed non-null default/fallback callback.
- GoogleTest CRT configuration on MSVC now matches project runtime to prevent `/MT` vs `/MD` link errors.

## [0.1.0] - 2026-04-05

### Added

- Portable fixed-width integer types (`lh_uint_t`, `lh_sint_t`, `lh_ushort_t`, `lh_sllong_t`, …)
- Character types with explicit signedness (`lh_uchar_t`, `lh_schar_t`)
- Boolean type (`lh_bool_t`)
- Void aliases (`lh_void`, `lh_void_ptr`)
- Compiler detection: type (Clang / GCC / MSVC), version, target OS, C++ mode
- `extern "C"` guards via `LH_COMPILER(EXTERN_C_BEGIN)` / `LH_COMPILER(EXTERN_C_END)`
- Constructor / destructor and other attribute helpers (`LH_ATTRIBUTE`, …)
- Type-safe cast macros: `lh_cast_static`, `lh_cast_reinterpret`
- Pointer helpers: `lh_ptr_of`, `lh_cptr_of`, and related macros
- String pointer aliases: `lh_str_ptr`, `lh_str_cptr` (`lh/str/ptr.h`)
- Compound-literal initializer macros (`lh/initializer.h`: `lh_initializer`, `lh_initializer_of_type`, …)
- Structured errors: `lh_error_t` with `lh_error_code_t` and `lh_error_desc_t` (`lh/error/desc.h`, alias of `lh_str_cptr` today)
- Exception value type `lh_exception_t`, debug `lh_exception_origin_t`, initializers under `lh/exception/`
- Runtime exception path: catch stack, throw / rethrow macros (`lh/runtime/…`)
- Non-owning memory views/ranges and numeric interval helpers
- `lh_version_t` and `lh_get_version()` (`lh/lh.h`)
- `LH_LIBRARY_OPTION_*` build-time options in generated `lh/config.h` (thread-local, terminate handler, catch-stack depth, …)
- `LH_OPTION_ON` / `LH_OPTION_OFF` symbolic constants for feature toggles
- CMake build with `LH_BUILD_SHARED`, `LH_BUILD_DOCS`, `LH_BUILD_TESTS`, optional Doxygen + Graphviz (`LH_DOCS_GRAPHS`)
- Tracked `include/lh/config.h` for manual builds; CMake emits a generated `lh/config.h` on the build include path so it overrides by order
- Doxygen documentation; GoogleTest-based test target

### Changed

- String-related public headers live under `lh/str/` (replacing the former `lh/string/` path)
- Generic initializer macros moved from `lh/initializer/initializer.h` to `lh/initializer.h`
- Error APIs use `lh_error_desc_t` for description pointers instead of hard-coded `const char *`

### Removed

- `LH_COMPILE_OPTION_*` layer (`compile_option.h`, related CMake modules): optimization, debug symbols, `NDEBUG`, and LTO are expected from the toolchain / CMake flags, not the public lh API

[Unreleased]: https://github.com/darwin128k/lh/compare/v0.2.0...HEAD
[0.2.0]: https://github.com/darwin128k/lh/releases/tag/v0.2.0
[0.1.0]: https://github.com/darwin128k/lh/releases/tag/v0.1.0
