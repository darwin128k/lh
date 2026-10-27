# Light Helper

<p align="center">
  <img src="docs/assets/logo.png" alt="lh — Light Helper" height="256"/>
  <br/>
  <img src="https://img.shields.io/badge/Compatible%20with-Windows%20Vista-1B6FDE?style=for-the-badge&logo=windows&logoColor=white" alt="Compatible with Windows Vista"/>
</p>

A lightweight C utility library (headers + a small static library) with portable numeric and character types, compiler portability macros, safe cast helpers, **structured errors** (`lh_error_t`), and **runtime checks** that report a failure as an `lh_exception_t` (error plus the check's origin) to a host-installed handler — no setjmp/longjmp, no unwinding.

lh is written in C11 and builds as one static library from CMake. Beyond the
core utilities it has an optional OS layer (files, directories, sockets, clock,
windows) and an optional UI model (`lh/ui/`). Float math is opt-in
(`LH_LIBRARY_OPTION_MATH_FPU`, default `OFF`). The same code targets a
microcontroller with no OS (STM32F407 + ILI9341), a desktop window (Win32 GDI
from Windows XP up, X11) and a browser — see [Usage](#usage).

## Features

- **Portable types** — fixed-width integers (`lh_uint_t`, `lh_sllong_t`, …), explicit `lh_byte_t` / `lh_schar_t`, `lh_bool_t`, `lh_void` / `lh_void_ptr`
- **Compiler layer** — detection (Clang / GCC / MSVC), version, target OS, C++ vs C, `extern "C"` via `LH_COMPILER(EXTERN_C_BEGIN|END)`, attributes (`LH_ATTRIBUTE(SYMBOL)`, …), **subsystem detection macros** (`lh/compiler/os/subsys.h`) for Windows-specific visibility attributes
- **Casts** — `lh_cast_static`, `lh_cast_reinterpret` (C++-style in C++, C casts in C)
- **Pointers** — `lh_ptr`, `lh_void_ptr`, `lh_ptr_of`, `lh_cptr_of`, `lh_str_ptr` / `lh_str_cptr` (`lh/ptr.h`, `lh/void/ptr.h`, `lh/util/ptr.h`, `lh/str/ptr.h`)
- **Compound literals** — `lh_initializer`, `lh_initializer_of_type`, … in `lh/initializer.h`
- **Member-offset utility** — `lh_offset_of` macro in `lh/offset.h` for compile-time layout checks
- **Macro utilities** — argument token concatenation (`lh_arg_concat`) and count (`lh_arg_get_count`) for variadic macro dispatch in `lh/util/arg.h`
- **Runtime assertions** — `lh_assert_runtime`, `lh_assert_runtime_if`, `lh_assert_runtime_ifn`, `lh_assert_runtime_ref` — contract checks in `lh/assert.h`, written as `(condition, code [, "message"])`: on failure an `lh_exception_t` (`lh/exception.h`: the error — code and message — plus its static origin — file, line, function, condition; amount set by `LH_LIBRARY_OPTION_RUNTIME_CHECK_REPORT`) goes to the host's handler (`lh_runtime_check_set`), then `lh_runtime_terminate` (`abort()` by default); compiled out in Release (`NDEBUG`), optionally a call-free trap (`LH_RUNTIME_ASSERT_TRAP`). `lh_runtime_check_*` are the always-on checks for environment failures (e.g. allocation).
- **Errors** — `lh_error_t` (code + description); description type is `lh_error_desc_t` in `lh/error/desc.h` (today an alias of `lh_str_cptr`)
- **Memory & intervals** — non-owning memory views (`lh_memory_view_t`) and bounds (`lh_memory_bounds_t`) with front/back and indexed access, validated and fallback APIs (`*_v`, `*_or_empty`); numeric interval helpers and binary-search macros used by table lookups
- **Memory allocator** — configurable alloc/dealloc callback pair (`lh_memory_allocator_t`, `lh/memory/allocator.h`) with pack/unpack/init/set and validated alloc/dealloc/realloc; a thread-local runtime singleton (`lh_runtime_allocator()`, `lh/runtime/allocator.h`) seeded at compile time from a configurable default pair (`malloc`/`free` unless overridden — see Library options below)
- **Heap-owning memory bounds** — `lh_memory_bounds_allocated_t` (`lh/memory/bounds/allocated.h`) is layout-identical to `lh_memory_bounds_t` but routes `clear` / `resize` / `exchange` through the runtime allocator, freeing or reallocating the owned block as needed
- **Growable containers** — `lh_array_t` (`lh/array.h`), a typed growable array (push/insert/erase/pop_back/reserve, amortized growth policy) backed by a heap-owning typed block; `lh_str_t` (`lh/str.h`), an owning NUL-terminated string built directly on `lh_array_t` (`init`/`deinit`, `push_back`/`append`, `clear`, conversion to `lh_str_view_t`)
- **Wide text** — `lh_wchar_t`, raw wide-string helpers, Unicode simple case mapping for buffers and single-code-point case fold (UCD-backed tables under `src/lh/util/wstr/` and `src/lh/util/wchar/`)
- **String search & compare** — `lh_str_view_find`, `lh_str_view_rfind`, `lh_str_view_find_of`, case-insensitive compare, size-aware comparison; same for wide strings (`lh_wstr_ptr_*`)
- **String formatting & parsing** — `lh_str_ptr_format_uint`/`_sint`/`_hex`/`_bytes_hex`/`_bytes_hex_dump`/`_text` and `lh_str_ptr_parse_uint`/`_hex` (`lh/str/format/`, `lh/str/parse/`); `lh_str_ptr_format_text`/`_v` is a printf-style engine used directly by the logger below
- **SIMD memory operations** — `lh/memory/std.h` is the byte kernel everything else is supposed to call (`copy` / `copy_rev` / `rcopy` / `set` / `compare` / `rcompare`). Those runtime-dispatch to a hand-written SSE2/SSSE3/AVX2 tier (resolved once via a self-rewriting function pointer) when the toolchain can compile it and the running CPU supports it, falling back to a portable scalar path everywhere else — see [Performance](#performance)
- **CPU feature/vendor detection** — `lh_cpu_simd_has_sse2/ssse3/avx2/aesni` (`lh/cpu/simd.h`), `lh_cpu_vendor_is_intel/is_amd` (`lh/cpu/vendor.h`)
- **Bit utilities** — header-only, force-inlined helpers under `lh/bit/`: `lh_bit_scan_forward/reverse` (`u8`/`u16`/`u32`/`u64`), `lh_bit_bswap_u16/u32/u64`, `lh_bit_rotate_left/right_u32/u64`, and big/little-endian `lh_bit_pack_be16/le16/be32/le32/be64/le64` / `lh_bit_unpack_*` for packing values into byte buffers
- **Logging** — `lh_logger_t` (`lh/logger.h`), a level-filtered logger (PSR-3-shaped severities, emergency through debug) with a runtime filter mask plus an independent callback slot per level, so different severities can be routed to different sinks without branching; `lh_logger_debug`/`_info`/`_warning`/… convenience macros and the explicit `lh_logger_log`/`lh_logger_log_v`
- **IO abstraction** — callback-based `lh_io_reader_t` / `lh_io_writer_t` and the duplex `lh_io_stream_t` that pairs them (`lh/io/`), so code written against read/write does not need to know whether a socket, file, or pipe is behind it
- **Networking** — pure value types for IPv4 (`lh_net_ip4_t` — parse/format/pack/unpack, `lh/net/ip.h`), a TCP/UDP port (`lh_net_port_t`, `lh/net/port.h`), and an address pair (`lh_net_ip4_socket_addr_t`, `lh/net/socket/addr/ip4.h`); a blocking IPv4 TCP/UDP client socket (`lh_os_net_socket_t`, `lh/os/net/socket.h`) that hands out an `lh_io_stream_t` — no bind/listen/accept or non-blocking mode yet
- **Crypto** — `lh_crypto_rijndael_t` (`lh/crypto/rijndael.h`): Rijndael/AES block cipher, 16/24/32-byte keys and blocks, ECB/CBC/CFB modes, pure computation (no OS, no allocator); 16-byte (AES) blocks run on AES-NI when the CPU has it (`lh_cpu_simd_has_aesni`), Rijndael's 24/32-byte blocks and other CPUs use the table path; `lh_crypto_sha_t` (`lh/crypto/sha.h`): SHA-1/224/256/384/512 (FIPS 180-4) with incremental `init`/`update`/`finish` and one-shot `lh_crypto_sha_hash`
- **Version** — `lh_version_t`, `lh_get_version()` via `lh/lh.h`
- **Build** — CMake, generated `lh/config.h`, optional Doxygen docs and bundled GoogleTest

## Requirements

| Tool        | Minimum version |
|-------------|-----------------|
| CMake       | 3.12 (3.13 with tests) |
| C compiler  | **C11** or later |
| C++ compiler | **C++17** for tests |
| Doxygen     | any (optional, for docs) |
| Graphviz    | any (optional, for graphs) |
| GoogleTest  | bundled submodule; required with `LH_BUILD_TESTS=ON` |

## Building

Initialize the pinned GoogleTest dependency before building with tests enabled:

```sh
git submodule update --init --recursive test/lib/googletest
```

All LH C/C++ unit tests use GoogleTest and the common runner under `test/src/`.
A missing dependency is a configure error when `LH_BUILD_TESTS=ON`; use
`-DLH_BUILD_TESTS=OFF` for a library-only build.

```sh
cmake -S . -B build
cmake --build build
```

Convenience scripts are available for common platform builds:

```sh
./scripts/build-linux.sh --config Release --run-tests
```

The script accepts options for docs/tests, target, build directory, compiler
selection, clean builds, and parallelism. See
[scripts/README.md](scripts/README.md) for the quick command list.

Legacy Windows builds that must work from `cmd.exe` can use the batch wrappers:

```bat
scripts\build-windows-xp.bat --clean
scripts\build-windows-vista.bat --clean
```

Details are in [scripts/README.md](scripts/README.md).

### Options

| CMake option | Default | Description |
|---|---|---|
| `LH_BUILD_DOCS` | `ON` | Generate Doxygen documentation |
| `LH_BUILD_TESTS` | `ON` | Build GoogleTest targets |
| `LH_BUILD_BENCH` | `OFF` | Build Google Benchmark microbenchmarks (`bench/lib/benchmark` submodule) |
| `LH_DOCS_GRAPHS` | `ON` | Include Graphviz dependency graphs |
| `LH_ENABLE_RELEASE_MAX_OPT` | `ON` | Aggressive Release optimizations for the library (`-O3`, section GC; `/O2 /Oi /Ot /Gy` on MSVC) |
| `LH_STATIC_LIBGCC` | `OFF` | GCC/Clang: pass `-static-libgcc` and `-static-libstdc++` to dependents (INTERFACE). For hosts that ship an older `libgcc_s` (e.g. HLDS) |
| `LH_OS_WINDOWS_VERSION` | `0x0501` | Windows only: `WINVER` / `_WIN32_WINNT` for lh's C and C++ (`0x0501` = XP, `0x0600` = Vista; four hex digits, `0x0501` or newer). The `scripts/build-windows-*.bat` wrappers set it |
| `LH_LIBRARY_OPTION_LTO` | `FAT` | Release LTO of the static library: `OFF` (machine code, links anywhere), `ON` (IR only, host must link with LTO and the same compiler), `FAT` (IR + machine code, links with or without LTO; falls back to `ON` on MSVC / Clang < 17) |

lh is built as a static library only.

Example — without docs, as plain machine code any host can link:

```sh
cmake -S . -B build -DLH_BUILD_DOCS=OFF -DLH_BUILD_TESTS=OFF -DLH_LIBRARY_OPTION_LTO=OFF
cmake --build build
```

### Library options

These control runtime *behavior* baked into the generated `lh/config.h` (see
`cmake/library_options.cmake`) — distinct from the build-shape options above.
Manual (no-CMake) builds set the same names directly in `config.h` or via `-D`.

| Option | Default | Description |
|---|---|---|
| `LH_LIBRARY_OPTION_THREAD_LOCAL` | `ON` | `LH_ATTRIBUTE_THREAD_LOCAL` uses real TLS in library sources |
| `LH_LIBRARY_OPTION_OS` | `ON` | Compile the OS layer (files, directories, sockets, clock); `OFF` for freestanding / bare-metal builds |
| `LH_LIBRARY_OPTION_OS_WERROR` | `ON` on Windows, else `OFF` | OS last-error slot uses `lh_werror_t` / `FormatMessageW` instead of `lh_error_t` / `FormatMessageA` |
| `LH_LIBRARY_OPTION_MATH_FPU` | `OFF` | `ON`: compile float math (`fpoint` / `fsize` / `frect`, `vec` / `quat` / `mat4`) on `lh_math_fscalar_t`; UI aliases those. `OFF`: UI aliases `lh_math_scalar_t` / `point` / `size` / `rect` |
| `LH_LIBRARY_OPTION_REAL` | `DOUBLE` | `lh_real_t` for `lh_value_t`: `DOUBLE`, `FLOAT`, or `NONE` (no real kind, no soft-float from values) |
| `LH_LIBRARY_OPTION_RUNTIME_TERMINATE_USE_STDLIB` | `ON` | Default `lh_runtime_terminate` handler is `abort()` and the default check handler prints to `stderr`; `OFF` leaves both to the host |
| `LH_LIBRARY_OPTION_RUNTIME_CHECK_REPORT` | `FULL` | What a failed runtime check reports (`lh_exception_origin_t`): `NONE` (error code only), `LOCATION` (+ file, line, message), `FULL` (+ function and condition text) |
| `LH_LIBRARY_OPTION_MEMORY_ALLOCATOR_USE_STDLIB` | `ON` | Seed the runtime allocator with the default functions below at startup; `OFF` leaves it null until `lh_memory_allocator_set()` is called |
| `LH_LIBRARY_OPTION_MEMORY_ALLOCATOR_DEFAULT_ALLOC` | `malloc` | Function bound as the runtime allocator's `alloc_cb` (e.g. `pvPortMalloc`) |
| `LH_LIBRARY_OPTION_MEMORY_ALLOCATOR_DEFAULT_DEALLOC` | `free` | Function bound as the runtime allocator's `dealloc_cb` (e.g. `vPortFree`) |
| `LH_LIBRARY_OPTION_MEMORY_ALLOCATOR_DEFAULT_REALLOC` | `realloc` with `malloc`/`free`, else `lh_null` | Function bound as the runtime allocator's optional `realloc_cb` |
| `LH_LIBRARY_OPTION_MEMORY_ALLOCATOR_DEFAULT_INCLUDE` | `<stdlib.h>` | Header declaring the functions above |
| `LH_LIBRARY_OPTION_MEMORY_ALLOCATOR_INIT_ALLOCATED` | `ON` | Zero-initialize memory returned by `lh_memory_allocator_alloc` |
| `LH_LIBRARY_OPTION_ARRAY_INITIAL_CAPACITY` | `1` | Capacity `lh_array_t` grows to from empty on its first insertion (must be `> 0`) |
| `LH_LIBRARY_OPTION_ARRAY_GROWTH_FACTOR` | `2` | Factor by which `lh_array_t` capacity grows when full (must be `> 1`) |
| `LH_LIBRARY_OPTION_STR_CASE_MAP_USE_TABLE` | `ON` | `lh_str_ptr_to_lower`/`_to_upper` use a dense 256-entry table (faster, +~400 B) instead of a binary search |
| `LH_LIBRARY_OPTION_WSTR_CASE_MAP_USE_TABLE` | `ON` | `lh_wstr_ptr_to_lower`/`_to_upper` use a two-level BMP table instead of a binary search |
| `LH_LIBRARY_OPTION_ALGORITHM_COMPARE_BLOCK` | `16` | Block size of the branchless scan in `lh_memory_std_compare` / `lh_memory_find_step` (must be `> 0`) |
| `LH_LIBRARY_OPTION_MEMORY_STD_SIMD_MIN_THRESHOLD` | `16` | Below this size (bytes), `lh_memory_std_copy`/`copy_rev`/`rcopy` skip SIMD entirely and use the scalar/tiny-copy path |
| `LH_LIBRARY_OPTION_MEMORY_STD_SIMD_SET_THRESHOLD` | `32` | Below this size (bytes), `lh_memory_std_set` skips SIMD |
| `LH_LIBRARY_OPTION_MEMORY_STD_SIMD_XOR_THRESHOLD` | `256` | Below this size (bytes), `lh_memory_std_xor` keeps the byte loop (auto-vectorized to SSE2) instead of the AVX2 tier |
| `LH_LIBRARY_OPTION_MEMORY_STD_SIMD_DIRECT_DISPATCH_THRESHOLD` | `256` | x86-64 only: below this size (bytes), call the SSE2 tier directly instead of going through the indirect, AVX2-capable dispatch |
| `LH_LIBRARY_OPTION_MEMORY_STD_SIMD_STREAM_THRESHOLD` | `2097152` | At/above this size (bytes, 2 MiB), `copy`/`set` switch to non-temporal streaming stores |
| `LH_LIBRARY_OPTION_MEMORY_STD_GCC_REP_MOVSB_THRESHOLD` | `512` | GCC/Clang x86 only: below this size, prefer SIMD over `REP MOVSB` in the (normally unused) no-SIMD fallback path |
| `LH_LIBRARY_OPTION_MEMORY_STD_PREFETCH_TRIGGER` | `4096` | Minimum remaining bytes before `lh_memory_std_copy` issues a prefetch |
| `LH_LIBRARY_OPTION_MEMORY_STD_PREFETCH_DISTANCE` | `256` | How many bytes ahead of the read position to prefetch |

Every `MEMORY_STD_*` value above is a measured crossover point (this project's own x86-64 GCC/MinGW and MSVC targets), not a correctness fact — retuning for a different microarchitecture is always safe to try. See the option comments in `cmake/library_options.cmake` for the full rationale.

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

Every guide is indexed in [docs/index.md](docs/index.md). The integration
guides by target — options, runtime wiring, display and UI:
[docs/usage.md](docs/usage.md) (overview and option matrix),
[microcontroller](docs/usage-embedded.md), [desktop window](docs/usage-desktop.md),
[web host](docs/usage-web.md).

The subsystem guides: [UI model](docs/ui.md),
[modules and loaders](docs/module-loader.md),
[OS threads and synchronization](docs/concurrency.md),
[HTTP/1 head parser](docs/http.md),
[software 3D renderer](docs/renderer3d.md).

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
lh_byte_t   byte    = 0xFF; /* 8-bit unsigned  */
```

### Version struct

```c
lh_version_t ver = lh_version_initializer(0, 3, 0);
lh_version_major_t major = lh_version_get_major(&ver); /* 0 */
```

## Performance

`lh/memory/std.h`'s `copy` / `copy_rev` / `rcopy` / `set` / `compare` / `rcompare`
dispatch at runtime to a hand-written SSE2/SSSE3/AVX2 tier when two things both
hold: the toolchain can compile it for the target (decided once at CMake
configure time by a compile-only probe — `cmake/check_simd.cmake` — never by
running code, so this stays correct under cross-compilation) and the CPU the
binary actually runs on supports it (`lh/cpu/simd.h`, checked once and cached
behind a self-rewriting function pointer). Everywhere either doesn't hold, it
falls back to the portable scalar path, unchanged.

The probe's answers (`LH_SIMD_HAVE_SSE2`, `_SSSE3`, `_AVX2`, `_AESNI`) can be
overridden on the CMake command line: a value given with `-D` skips that probe.
`-DLH_SIMD_HAVE_AESNI=0`, for example, builds lh without the AES-NI tier, so
AES runs on the table path even on a CPU that has AES-NI.

Throughput against the platform CRT, measured on this project's own x86-64
GCC/MinGW Release build (`CMAKE_BUILD_TYPE=Release`, SSE2 present, no AVX2 —
an AVX2-capable CPU pushes the SIMD numbers further still):

```
                                                      GB/s
copy     64 B   lh #############                    21.4  crt #########                        15.3
copy     4 KB   lh ##############################  108.9  crt #################               61.0
copy     64 KB  lh ##################               65.3  crt ############                    43.8
copy     1 MB   lh ##############                   52.3  crt ###                             10.5
copy     16 MB  lh ##                                7.5  crt ###                             11.3
set      64 B   lh #########                        15.3  crt ########                        14.2
set      4 KB   lh ##############################  109.4  crt #################               61.3
set      64 KB  lh ##############################  112.0  crt #################               65.3
set      1 MB   lh ######################           83.6  crt ###############                 56.0
set      16 MB  lh #####                            18.0  crt ##                               9.0
compare  4 KB   lh #################                61.3  crt #######                         27.3
compare  64 KB  lh ###############                  55.8  crt ########                        29.2
compare  1 MB   lh #############                    46.7  crt ########                        28.0
xor      4 KB   lh ####################             72.9
xor      1 MB   lh ##########                       35.0
```

No universal winner: `lh` tends to pull ahead in the mid-size SIMD band (its
hand kernels avoid the platform CRT's own dispatch overhead) and on large
fills/compares; at multi-megabyte copies the CRT can still win depending on
cache and streaming thresholds. Reproduce or extend these numbers with
`lh_bench --benchmark_filter=memory_std` (see [bench/](bench)); tune the
crossover points for your own CPU via the `LH_LIBRARY_OPTION_MEMORY_STD_*`
options above.

Other hot paths from the same Release run (`lh_bench`, ~330 cases):

```
                                         time / note
SHA-256         64 B                    456 ns   (~117 MB/s)
SHA-256         4 KB                     14 µs   (~278 MB/s)
SHA-256         1 MB                    4.2 ms   (~288 MB/s)
AES-128 ECB     encrypt block           323 ns   (~44 MB/s)
AES-256 ECB     encrypt block           451 ns   (~36 MB/s)
AES-128         key schedule             68 ns
Modbus CRC-16   64 B                    497 ns   (~98 MB/s)
list_sort       1 000 ints               18 µs
list_sort       10 000 ints             354 µs
array_push      1 000 reserved          2.3 µs
str_append      1 000×10 chars          4.6 µs
fs_path_join                            23 ns
logger_info     (sink enabled)          51 ns
logger_debug    (filtered out)         1.5 ns
rect_contains                           3.1 ns
bswap_u32                             0.23 ns
```

Build and run the full suite:

```bat
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DLH_BUILD_BENCH=ON
cmake --build build --config Release --target lh_bench
build\bench\lh_bench.exe --benchmark_filter=crypto
build\bench\lh_bench.exe --benchmark_list_tests
```

## Safety conventions

Most public APIs in this library validate their arguments: a broken contract
(null pointer, out-of-range offset, invalid range) is reported as an
`lh_exception_t` to the check handler and ends the program
(`lh_runtime_terminate`). These contract checks are compiled in while
`LH_RUNTIME_ASSERT_ENABLED` is `1` — by default in builds without `NDEBUG` —
and compiled out in Release, where breaking such a contract is undefined
behaviour rather than a reported error. Pointer parameters must be non-null
even when the matching size is `0`: `lh_str_ptr_find_of_chars(p, n, NULL, 0)`
breaks the contract the same way a non-empty call would. Expected failures (a missing file, a closed
socket) are returned as results instead. A small number of functions are intentionally
**unsafe** — they skip range validation to let callers who have already
verified invariants avoid redundant checks. These functions are explicitly
marked with a `@warning` tag in their documentation.

Current unsafe functions:

| Function | What is skipped |
|---|---|
| `lh_memory_bounds_set` | range check on `[begin, end)` |
| `lh_memory_bounds_swap` | range check on both operands |
| `lh_memory_view_set` | range check on `[begin, end)` |

Null-pointer checks are still enforced in unsafe functions whenever contract
checks are compiled in.

**When calling an unsafe function the caller assumes full responsibility** for
the validity of the range. Passing a backward or otherwise invalid range
produces a logically inconsistent slice and may cause failures in any
subsequent operation that requires a valid slice.

## UI

`LH_LIBRARY_OPTION_UI` (default `ON`) compiles the UI model under `lh/ui/`:
color, paint, gradient, shadow, blur, style, transition, layout, canvas and the
entity tree. It needs no OS; the host supplies where the pixels go.

- **View** — `lh_ui_view_t` (`lh/ui/view.h`) binds an entity tree to a canvas,
  draws it (`lh_ui_view_draw`), takes input (press / move / release / wheel /
  key / text, or one `lh_os_input_event_t` through `lh_ui_view_input`) and
  accumulates damage for partial redraw.
- **Input** — `lh/os/input/` (no OS needed): one event type
  `lh_os_input_event_t` for pointer, wheel, key, text, controller and focus
  input, one sink callback, a single-producer ring `lh_os_input_queue_t` for
  events from an interrupt or a thread, and the gesture recognizer
  `lh_os_input_recognizer_t` (click count, double click, long press, drag).
  A desktop window delivers its input to one sink
  (`lh_os_window_set_on_input`), the web host to its `input` hook.
- **Canvas backends** — software, into a pixel buffer (`lh/ui/canvas/sw.h`);
  GDI, the same software renderer plus one `BitBlt` into a window
  (`lh/os/render/backend/gdi.h`); web, recording primitives as JSON
  (`lh/ui/canvas/web.h`).
- **Present sink** (`lh/ui/present/sink.h`) — shows an already drawn buffer
  on a TFT, a log or a second screen.
- **Scalar mode** — `LH_LIBRARY_OPTION_MATH_FPU` picks the UI coordinate type
  `lh_ui_scalar_t` (`lh/ui/scalar.h`): `OFF` uses integer `lh_math_scalar_t`,
  `ON` uses float `lh_math_fscalar_t`.
- **Font** — `LH_LIBRARY_OPTION_UI_FONT_ROBOTO` (default `ON`) bakes Roboto
  16 px, 4 bpp, Latin + Cyrillic; it also builds with UI `OFF`.
- **Limits** — `LH_LIBRARY_OPTION_UI_GRADIENT_MAX_STOPS` (default `2`) and
  `LH_LIBRARY_OPTION_UI_CANVAS_DEPTH` (default `8`, nested `lh_ui_canvas_push`
  levels).

Frame times against LVGL on the same scenes are in
[bench/lvgl/RESULTS.md](bench/lvgl/RESULTS.md).

## Scene

A screen, a plain box, a label, a button and a circle ship with the library,
plus a style and three paint effects: shadow, blur and glass. A widget or an
effect the library does not have is a class written in the caller's program.
Nothing has to be registered, and the library does not have to be edited.

How to build a tree of them — by hand or from a stylesheet with
`scripts/css2decl.py` — is in [docs/usage-desktop.md](docs/usage-desktop.md).

## Version

Current version: **0.4.0** — history and release notes in [CHANGELOG](CHANGELOG.md).

Documentation for tools is in [tools/README.md](tools/README.md).

## License

See [LICENSE](LICENSE.md) for details.
