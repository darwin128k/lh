/**
 * @file fallback.h
 * @brief Default fallback values for library build options.
 *
 * This header provides default values for optional library features
 * when not explicitly configured via CMake or manual definition.
 *
 * These are the values of a build that has no `lh/config.h` setting for an
 * option. Feature switches that need a host (OS, stdlib hooks, TLS) are mostly
 * "off", unlike the desktop-oriented CMake defaults (`cmake/library_options.cmake`);
 * tuning values and struct-layout sizes match the CMake defaults.
 */

#ifndef LH_LIBRARY_OPTION_FALLBACK_H
#define LH_LIBRARY_OPTION_FALLBACK_H

#include <lh/config.h>
#include <lh/library/option.h>

/**
 * @def LH_LIBRARY_OPTION_THREAD_LOCAL
 * @brief Library sources use real thread-local storage. Fallback: off.
 */
#ifndef LH_LIBRARY_OPTION_THREAD_LOCAL
#    define LH_LIBRARY_OPTION_THREAD_LOCAL LH_LIBRARY_OPTION_OFF
#endif /* LH_LIBRARY_OPTION_THREAD_LOCAL */

/**
 * @def LH_LIBRARY_OPTION_OS
 * @brief The OS layer (files, sockets, clock, windows) is compiled. Fallback: off.
 */
#ifndef LH_LIBRARY_OPTION_OS
#    define LH_LIBRARY_OPTION_OS LH_LIBRARY_OPTION_OFF
#endif /* LH_LIBRARY_OPTION_OS */

/**
 * @def LH_LIBRARY_OPTION_OS_WERROR
 * @brief The OS last-error slot is wide (::lh_werror_t). Fallback: off.
 */
#ifndef LH_LIBRARY_OPTION_OS_WERROR
#    define LH_LIBRARY_OPTION_OS_WERROR LH_LIBRARY_OPTION_OFF
#endif /* LH_LIBRARY_OPTION_OS_WERROR */

/**
 * @def LH_LIBRARY_OPTION_MATH_FPU
 * @brief Float math is compiled and UI geometry uses it. Fallback: off.
 */
#ifndef LH_LIBRARY_OPTION_MATH_FPU
#    define LH_LIBRARY_OPTION_MATH_FPU LH_LIBRARY_OPTION_OFF
#endif /* LH_LIBRARY_OPTION_MATH_FPU */

/**
 * @def LH_LIBRARY_OPTION_RUNTIME_TERMINATE_USE_STDLIB
 * @brief ::lh_runtime_terminate starts as `abort()`. Fallback: off (the host installs it).
 */
#ifndef LH_LIBRARY_OPTION_RUNTIME_TERMINATE_USE_STDLIB
#    define LH_LIBRARY_OPTION_RUNTIME_TERMINATE_USE_STDLIB LH_LIBRARY_OPTION_OFF
#endif /* LH_LIBRARY_OPTION_RUNTIME_TERMINATE_USE_STDLIB */

/**
 * @def LH_LIBRARY_OPTION_RUNTIME_IO_USE_STDLIB
 * @brief The runtime writer and reader start as stdout / stdin. Fallback: off.
 */
#ifndef LH_LIBRARY_OPTION_RUNTIME_IO_USE_STDLIB
#    define LH_LIBRARY_OPTION_RUNTIME_IO_USE_STDLIB LH_LIBRARY_OPTION_OFF
#endif /* LH_LIBRARY_OPTION_RUNTIME_IO_USE_STDLIB */

/**
 * @def LH_LIBRARY_OPTION_RUNTIME_CHECK_REPORT
 * @brief What a failed runtime check reports: 0 none, 1 location, 2 full. Fallback: 2 (full).
 */
#ifndef LH_LIBRARY_OPTION_RUNTIME_CHECK_REPORT
#    define LH_LIBRARY_OPTION_RUNTIME_CHECK_REPORT 2 /* LH_RUNTIME_CHECK_REPORT_FULL */
#endif /* LH_LIBRARY_OPTION_RUNTIME_CHECK_REPORT */

/**
 * @def LH_LIBRARY_OPTION_MEMORY_ALLOCATOR_USE_STDLIB
 * @brief The runtime allocator starts bound to the default functions below. Fallback: off.
 */
#ifndef LH_LIBRARY_OPTION_MEMORY_ALLOCATOR_USE_STDLIB
#    define LH_LIBRARY_OPTION_MEMORY_ALLOCATOR_USE_STDLIB LH_LIBRARY_OPTION_OFF
#endif /* LH_LIBRARY_OPTION_MEMORY_ALLOCATOR_USE_STDLIB */

/**
 * @def LH_LIBRARY_OPTION_MEMORY_ALLOCATOR_DEFAULT_ALLOC
 * @brief Function bound as the runtime allocator's `alloc`. Fallback: `malloc`.
 */
#ifndef LH_LIBRARY_OPTION_MEMORY_ALLOCATOR_DEFAULT_ALLOC
#    define LH_LIBRARY_OPTION_MEMORY_ALLOCATOR_DEFAULT_ALLOC malloc
#endif /* LH_LIBRARY_OPTION_MEMORY_ALLOCATOR_DEFAULT_ALLOC */

/**
 * @def LH_LIBRARY_OPTION_MEMORY_ALLOCATOR_DEFAULT_DEALLOC
 * @brief Function bound as the runtime allocator's `dealloc`. Fallback: `free`.
 */
#ifndef LH_LIBRARY_OPTION_MEMORY_ALLOCATOR_DEFAULT_DEALLOC
#    define LH_LIBRARY_OPTION_MEMORY_ALLOCATOR_DEFAULT_DEALLOC free
#endif /* LH_LIBRARY_OPTION_MEMORY_ALLOCATOR_DEFAULT_DEALLOC */

/**
 * @def LH_LIBRARY_OPTION_MEMORY_ALLOCATOR_DEFAULT_INCLUDE
 * @brief Header that declares the default allocator functions. Fallback: `<stdlib.h>`.
 */
#ifndef LH_LIBRARY_OPTION_MEMORY_ALLOCATOR_DEFAULT_INCLUDE
#    define LH_LIBRARY_OPTION_MEMORY_ALLOCATOR_DEFAULT_INCLUDE <stdlib.h>
#endif /* LH_LIBRARY_OPTION_MEMORY_ALLOCATOR_DEFAULT_INCLUDE */

/**
 * @def LH_LIBRARY_OPTION_MEMORY_ALLOCATOR_INIT_ALLOCATED
 * @brief ::lh_memory_allocator_alloc zero-fills what it returns. Fallback: off.
 */
#ifndef LH_LIBRARY_OPTION_MEMORY_ALLOCATOR_INIT_ALLOCATED
#    define LH_LIBRARY_OPTION_MEMORY_ALLOCATOR_INIT_ALLOCATED LH_LIBRARY_OPTION_OFF
#endif /* LH_LIBRARY_OPTION_MEMORY_ALLOCATOR_INIT_ALLOCATED */

/**
 * @def LH_LIBRARY_OPTION_OS_WINDOW
 * @brief The native window backend is compiled. Fallback: follows ::LH_LIBRARY_OPTION_OS
 *        (as in CMake), so off unless the OS layer is on.
 */
#ifndef LH_LIBRARY_OPTION_OS_WINDOW
#    define LH_LIBRARY_OPTION_OS_WINDOW LH_LIBRARY_OPTION_OS
#endif /* LH_LIBRARY_OPTION_OS_WINDOW */

/**
 * @def LH_LIBRARY_OPTION_RUNTIME_LOGGER_USE_STDLIB
 * @brief The runtime logger starts writing to stderr. Fallback: off (the host installs it),
 *        like the other `*_USE_STDLIB` options here.
 */
#ifndef LH_LIBRARY_OPTION_RUNTIME_LOGGER_USE_STDLIB
#    define LH_LIBRARY_OPTION_RUNTIME_LOGGER_USE_STDLIB LH_LIBRARY_OPTION_OFF
#endif /* LH_LIBRARY_OPTION_RUNTIME_LOGGER_USE_STDLIB */

/**
 * @def LH_LIBRARY_OPTION_MEMORY_ALLOCATOR_DEFAULT_REALLOC
 * @brief Function bound as the runtime allocator's `realloc`. Fallback: `realloc`, the
 *        CMake default for the `malloc` / `free` pair above.
 */
#ifndef LH_LIBRARY_OPTION_MEMORY_ALLOCATOR_DEFAULT_REALLOC
#    define LH_LIBRARY_OPTION_MEMORY_ALLOCATOR_DEFAULT_REALLOC realloc
#endif /* LH_LIBRARY_OPTION_MEMORY_ALLOCATOR_DEFAULT_REALLOC */

/**
 * @def LH_LIBRARY_OPTION_MEMORY_ALLOCATOR_DEFAULT_HAS_REALLOC
 * @brief 1 when ::LH_LIBRARY_OPTION_MEMORY_ALLOCATOR_DEFAULT_REALLOC names a function. Fallback: 1.
 */
#ifndef LH_LIBRARY_OPTION_MEMORY_ALLOCATOR_DEFAULT_HAS_REALLOC
#    define LH_LIBRARY_OPTION_MEMORY_ALLOCATOR_DEFAULT_HAS_REALLOC 1
#endif /* LH_LIBRARY_OPTION_MEMORY_ALLOCATOR_DEFAULT_HAS_REALLOC */

/**
 * @def LH_LIBRARY_OPTION_UI
 * @brief The UI model under `lh/ui` is compiled. Fallback: on (CMake default).
 */
#ifndef LH_LIBRARY_OPTION_UI
#    define LH_LIBRARY_OPTION_UI LH_LIBRARY_OPTION_ON
#endif /* LH_LIBRARY_OPTION_UI */

/**
 * @def LH_LIBRARY_OPTION_UI_FONT_ROBOTO
 * @brief Roboto 16 px is baked in. Fallback: on (CMake default).
 */
#ifndef LH_LIBRARY_OPTION_UI_FONT_ROBOTO
#    define LH_LIBRARY_OPTION_UI_FONT_ROBOTO LH_LIBRARY_OPTION_ON
#endif /* LH_LIBRARY_OPTION_UI_FONT_ROBOTO */

/**
 * @def LH_LIBRARY_OPTION_UI_GRADIENT_MAX_STOPS
 * @brief Capacity of ::lh_ui_gradient_t color stops (1..255; changes the struct layout).
 *        Fallback: 2 (CMake default).
 */
#ifndef LH_LIBRARY_OPTION_UI_GRADIENT_MAX_STOPS
#    define LH_LIBRARY_OPTION_UI_GRADIENT_MAX_STOPS 2
#endif /* LH_LIBRARY_OPTION_UI_GRADIENT_MAX_STOPS */

/**
 * @def LH_LIBRARY_OPTION_UI_CANVAS_DEPTH
 * @brief Depth of the ::lh_ui_canvas_t offset / clip stack (1..255; changes the struct layout).
 *        Fallback: 8 (CMake default).
 */
#ifndef LH_LIBRARY_OPTION_UI_CANVAS_DEPTH
#    define LH_LIBRARY_OPTION_UI_CANVAS_DEPTH 8
#endif /* LH_LIBRARY_OPTION_UI_CANVAS_DEPTH */

/**
 * @def LH_LIBRARY_OPTION_ARRAY_INITIAL_CAPACITY
 * @brief Capacity ::lh_array_t grows to on its first insertion. Fallback: 1 (CMake default).
 */
#ifndef LH_LIBRARY_OPTION_ARRAY_INITIAL_CAPACITY
#    define LH_LIBRARY_OPTION_ARRAY_INITIAL_CAPACITY 1
#endif /* LH_LIBRARY_OPTION_ARRAY_INITIAL_CAPACITY */

/**
 * @def LH_LIBRARY_OPTION_ARRAY_GROWTH_FACTOR
 * @brief Factor ::lh_array_t capacity grows by once full. Fallback: 2 (CMake default).
 */
#ifndef LH_LIBRARY_OPTION_ARRAY_GROWTH_FACTOR
#    define LH_LIBRARY_OPTION_ARRAY_GROWTH_FACTOR 2
#endif /* LH_LIBRARY_OPTION_ARRAY_GROWTH_FACTOR */

/**
 * @def LH_LIBRARY_OPTION_ALGORITHM_COMPARE_BLOCK
 * @brief Block size of the branchless compare / find scan. Fallback: 16 (CMake default).
 */
#ifndef LH_LIBRARY_OPTION_ALGORITHM_COMPARE_BLOCK
#    define LH_LIBRARY_OPTION_ALGORITHM_COMPARE_BLOCK 16
#endif /* LH_LIBRARY_OPTION_ALGORITHM_COMPARE_BLOCK */

/**
 * @def LH_LIBRARY_OPTION_STR_CASE_MAP_USE_TABLE
 * @brief Narrow case mapping uses the dense table. Fallback: on (CMake default).
 */
#ifndef LH_LIBRARY_OPTION_STR_CASE_MAP_USE_TABLE
#    define LH_LIBRARY_OPTION_STR_CASE_MAP_USE_TABLE LH_LIBRARY_OPTION_ON
#endif /* LH_LIBRARY_OPTION_STR_CASE_MAP_USE_TABLE */

/**
 * @def LH_LIBRARY_OPTION_WSTR_CASE_MAP_USE_TABLE
 * @brief Wide case mapping uses the BMP block table. Fallback: on (CMake default).
 */
#ifndef LH_LIBRARY_OPTION_WSTR_CASE_MAP_USE_TABLE
#    define LH_LIBRARY_OPTION_WSTR_CASE_MAP_USE_TABLE LH_LIBRARY_OPTION_ON
#endif /* LH_LIBRARY_OPTION_WSTR_CASE_MAP_USE_TABLE */

/**
 * @def LH_LIBRARY_OPTION_MEMORY_STD_BACKEND_LIBC
 * @brief 1 when copy / move / rcopy / set wrap libc. Fallback: 0 (lh's own tiers, CMake `AUTO`).
 */
#ifndef LH_LIBRARY_OPTION_MEMORY_STD_BACKEND_LIBC
#    define LH_LIBRARY_OPTION_MEMORY_STD_BACKEND_LIBC 0
#endif /* LH_LIBRARY_OPTION_MEMORY_STD_BACKEND_LIBC */

/**
 * @def LH_LIBRARY_OPTION_MEMORY_STD_SIMD_MIN_THRESHOLD
 * @brief Below this size copy skips SIMD. Fallback: 16 (CMake default).
 */
#ifndef LH_LIBRARY_OPTION_MEMORY_STD_SIMD_MIN_THRESHOLD
#    define LH_LIBRARY_OPTION_MEMORY_STD_SIMD_MIN_THRESHOLD 16
#endif /* LH_LIBRARY_OPTION_MEMORY_STD_SIMD_MIN_THRESHOLD */

/**
 * @def LH_LIBRARY_OPTION_MEMORY_STD_SIMD_SET_THRESHOLD
 * @brief Below this size set skips SIMD. Fallback: 32 (CMake default).
 */
#ifndef LH_LIBRARY_OPTION_MEMORY_STD_SIMD_SET_THRESHOLD
#    define LH_LIBRARY_OPTION_MEMORY_STD_SIMD_SET_THRESHOLD 32
#endif /* LH_LIBRARY_OPTION_MEMORY_STD_SIMD_SET_THRESHOLD */

/**
 * @def LH_LIBRARY_OPTION_MEMORY_STD_SIMD_XOR_THRESHOLD
 * @brief Below this size xor keeps the byte loop. Fallback: 256 (CMake default).
 */
#ifndef LH_LIBRARY_OPTION_MEMORY_STD_SIMD_XOR_THRESHOLD
#    define LH_LIBRARY_OPTION_MEMORY_STD_SIMD_XOR_THRESHOLD 256
#endif /* LH_LIBRARY_OPTION_MEMORY_STD_SIMD_XOR_THRESHOLD */

/**
 * @def LH_LIBRARY_OPTION_MEMORY_STD_SIMD_DIRECT_DISPATCH_THRESHOLD
 * @brief Below this size x86-64 copy calls SSE2 directly. Fallback: 256 (CMake default).
 */
#ifndef LH_LIBRARY_OPTION_MEMORY_STD_SIMD_DIRECT_DISPATCH_THRESHOLD
#    define LH_LIBRARY_OPTION_MEMORY_STD_SIMD_DIRECT_DISPATCH_THRESHOLD 256
#endif /* LH_LIBRARY_OPTION_MEMORY_STD_SIMD_DIRECT_DISPATCH_THRESHOLD */

/**
 * @def LH_LIBRARY_OPTION_MEMORY_STD_SIMD_STREAM_THRESHOLD
 * @brief Floor for the non-temporal copy / set tier. Fallback: 2097152 (CMake default).
 */
#ifndef LH_LIBRARY_OPTION_MEMORY_STD_SIMD_STREAM_THRESHOLD
#    define LH_LIBRARY_OPTION_MEMORY_STD_SIMD_STREAM_THRESHOLD 2097152
#endif /* LH_LIBRARY_OPTION_MEMORY_STD_SIMD_STREAM_THRESHOLD */

/**
 * @def LH_LIBRARY_OPTION_MEMORY_STD_GCC_REP_MOVSB_THRESHOLD
 * @brief GCC/Clang x86 no-SIMD copy: below this size avoid REP MOVSB. Fallback: 512 (CMake default).
 */
#ifndef LH_LIBRARY_OPTION_MEMORY_STD_GCC_REP_MOVSB_THRESHOLD
#    define LH_LIBRARY_OPTION_MEMORY_STD_GCC_REP_MOVSB_THRESHOLD 512
#endif /* LH_LIBRARY_OPTION_MEMORY_STD_GCC_REP_MOVSB_THRESHOLD */

/**
 * @def LH_LIBRARY_OPTION_MEMORY_STD_USE_ERMS
 * @brief Intel+ERMS mid-size copy uses `rep movsb`. Fallback: on (CMake default).
 */
#ifndef LH_LIBRARY_OPTION_MEMORY_STD_USE_ERMS
#    define LH_LIBRARY_OPTION_MEMORY_STD_USE_ERMS LH_LIBRARY_OPTION_ON
#endif /* LH_LIBRARY_OPTION_MEMORY_STD_USE_ERMS */

/**
 * @def LH_LIBRARY_OPTION_MEMORY_STD_ERMS_COPY_THRESHOLD
 * @brief ERMS copy band starts at this size. Fallback: 16384 (CMake default).
 */
#ifndef LH_LIBRARY_OPTION_MEMORY_STD_ERMS_COPY_THRESHOLD
#    define LH_LIBRARY_OPTION_MEMORY_STD_ERMS_COPY_THRESHOLD 16384
#endif /* LH_LIBRARY_OPTION_MEMORY_STD_ERMS_COPY_THRESHOLD */

/**
 * @def LH_LIBRARY_OPTION_MEMORY_STD_PREFETCH_TRIGGER
 * @brief Minimum remaining bytes before copy prefetches. Fallback: 4096 (CMake default).
 */
#ifndef LH_LIBRARY_OPTION_MEMORY_STD_PREFETCH_TRIGGER
#    define LH_LIBRARY_OPTION_MEMORY_STD_PREFETCH_TRIGGER 4096
#endif /* LH_LIBRARY_OPTION_MEMORY_STD_PREFETCH_TRIGGER */

/**
 * @def LH_LIBRARY_OPTION_MEMORY_STD_PREFETCH_DISTANCE
 * @brief Prefetch distance of the SSE2 copy. Fallback: 256 (CMake default).
 */
#ifndef LH_LIBRARY_OPTION_MEMORY_STD_PREFETCH_DISTANCE
#    define LH_LIBRARY_OPTION_MEMORY_STD_PREFETCH_DISTANCE 256
#endif /* LH_LIBRARY_OPTION_MEMORY_STD_PREFETCH_DISTANCE */

/* LH_LIBRARY_OPTION_SIMD_HAVE_SSE2 / _SSSE3 / _AVX2 / _AESNI have no fallback on
   purpose: they are toolchain probes (cmake/check_simd.cmake) and AESNI changes
   lh_crypto_rijndael_t's layout, so a config.h without them is an error
   (lh/crypto/rijndael.h) rather than a silent guess. */

#endif /* LH_LIBRARY_OPTION_FALLBACK_H */
