/**
 * @file subsys.h
 * @brief Target subsystem detection macros.
 *
 * Provides subsystem identifier constants (::LH_COMPILER_OS_SUBSYS_UNKNOWN,
 * ::LH_COMPILER_OS_SUBSYS_CYGWIN, ::LH_COMPILER_OS_SUBSYS_MINGW,
 * ::LH_COMPILER_OS_SUBSYS_MSYS, ::LH_COMPILER_OS_SUBSYS_WINE,
 * ::LH_COMPILER_OS_SUBSYS_UWP) and ::LH_COMPILER_OS_SUBSYS
 * — a compile-time constant set to the detected subsystem.
 *
 * @note All macros can be overridden by defining them before including this header.
 */

#ifndef LH_COMPILER_OS_SUBSYS_H
#define LH_COMPILER_OS_SUBSYS_H

#include <lh/compiler/os.h>

#ifndef LH_COMPILER_OS_SUBSYS_UNKNOWN
/**
 * @def LH_COMPILER_OS_SUBSYS_UNKNOWN
 * @brief Subsystem identifier for an unrecognised target (0).
 */
#    define LH_COMPILER_OS_SUBSYS_UNKNOWN 0
#endif /* LH_COMPILER_OS_SUBSYS_UNKNOWN */

#ifndef LH_COMPILER_OS_SUBSYS_CYGWIN
/**
 * @def LH_COMPILER_OS_SUBSYS_CYGWIN
 * @brief Subsystem identifier for Cygwin (1).
 */
#    define LH_COMPILER_OS_SUBSYS_CYGWIN 1
#endif /* LH_COMPILER_OS_SUBSYS_CYGWIN */

#ifndef LH_COMPILER_OS_SUBSYS_MINGW
/**
 * @def LH_COMPILER_OS_SUBSYS_MINGW
 * @brief Subsystem identifier for MinGW (2).
 */
#    define LH_COMPILER_OS_SUBSYS_MINGW 2
#endif /* LH_COMPILER_OS_SUBSYS_MINGW */

#ifndef LH_COMPILER_OS_SUBSYS_MSYS
/**
 * @def LH_COMPILER_OS_SUBSYS_MSYS
 * @brief Subsystem identifier for MSYS (3).
 */
#    define LH_COMPILER_OS_SUBSYS_MSYS 3
#endif /* LH_COMPILER_OS_SUBSYS_MSYS */

#ifndef LH_COMPILER_OS_SUBSYS_WINE
/**
 * @def LH_COMPILER_OS_SUBSYS_WINE
 * @brief Subsystem identifier for Wine (4).
 */
#    define LH_COMPILER_OS_SUBSYS_WINE 4
#endif /* LH_COMPILER_OS_SUBSYS_WINE */

#ifndef LH_COMPILER_OS_SUBSYS_UWP
/**
 * @def LH_COMPILER_OS_SUBSYS_UWP
 * @brief Subsystem identifier for Universal Windows Platform (5).
 */
#    define LH_COMPILER_OS_SUBSYS_UWP 5
#endif /* LH_COMPILER_OS_SUBSYS_UWP */

#ifndef LH_COMPILER_OS_SUBSYS
/**
 * @def LH_COMPILER_OS_SUBSYS
 * @brief Detected target subsystem.
 *
 * Set to one of the subsystem identifier constants based on predefined macros.
 * Only meaningful when ::LH_COMPILER_OS is ::LH_COMPILER_OS_WINDOWS.
 *
 * | Value                           | Detection macro              |
 * |---------------------------------|------------------------------|
 * | ::LH_COMPILER_OS_SUBSYS_CYGWIN  | `__CYGWIN__`                 |
 * | ::LH_COMPILER_OS_SUBSYS_MINGW   | `__MINGW32__`, `__MINGW64__` |
 * | ::LH_COMPILER_OS_SUBSYS_MSYS    | `__MSYS__`                   |
 * | ::LH_COMPILER_OS_SUBSYS_WINE    | `__WINE__`                   |
 * | ::LH_COMPILER_OS_SUBSYS_UWP     | `_UWP`                       |
 * | ::LH_COMPILER_OS_SUBSYS_UNKNOWN | *(none of the above)*        |
 *
 * Example usage:
 * @code{.c}
 * #if LH_COMPILER_OS_SUBSYS == LH_COMPILER_OS_SUBSYS_CYGWIN
 *     // Cygwin-specific code
 * #endif
 * @endcode
 */
#    if defined(__CYGWIN__)
#        define LH_COMPILER_OS_SUBSYS LH_COMPILER_OS_SUBSYS_CYGWIN
#    elif defined(__MINGW32__) || defined(__MINGW64__)
#        define LH_COMPILER_OS_SUBSYS LH_COMPILER_OS_SUBSYS_MINGW
#    elif defined(__MSYS__)
#        define LH_COMPILER_OS_SUBSYS LH_COMPILER_OS_SUBSYS_MSYS
#    elif defined(__WINE__)
#        define LH_COMPILER_OS_SUBSYS LH_COMPILER_OS_SUBSYS_WINE
#    elif defined(_UWP)
#        define LH_COMPILER_OS_SUBSYS LH_COMPILER_OS_SUBSYS_UWP
#    else
#        define LH_COMPILER_OS_SUBSYS LH_COMPILER_OS_SUBSYS_UNKNOWN
#    endif
#endif /* LH_COMPILER_OS_SUBSYS */

#endif /* LH_COMPILER_OS_SUBSYS_H */
