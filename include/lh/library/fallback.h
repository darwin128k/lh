/**
 * @file fallback.h
 * @brief Default fallback values for library build options.
 *
 * This header provides default values for optional library features
 * when not explicitly configured via CMake or manual definition.
 *
 * These are the values of a build that has no `lh/config.h` setting for an
 * option, which is mostly "off": the CMake defaults (`cmake/library_options.cmake`)
 * are a different, desktop-oriented set.
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

#endif /* LH_LIBRARY_OPTION_FALLBACK_H */
