/**
 * @file os.h
 * @brief Target operating system detection macros.
 *
 * Provides OS identifier constants (::LH_COMPILER_OS_UNKNOWN,
 * ::LH_COMPILER_OS_WINDOWS, ::LH_COMPILER_OS_LINUX, ::LH_COMPILER_OS_MAC)
 * and ::LH_COMPILER_OS — a compile-time constant set to the detected OS.
 *
 * @note All macros can be overridden by defining them before including this header.
 */

#ifndef LH_COMPILER_OS_H
#define LH_COMPILER_OS_H

#ifndef LH_COMPILER_OS_UNKNOWN
/**
 * @def LH_COMPILER_OS_UNKNOWN
 * @brief OS identifier for an unrecognised target (0).
 */
#    define LH_COMPILER_OS_UNKNOWN 0
#endif /* LH_COMPILER_OS_UNKNOWN */

#ifndef LH_COMPILER_OS_WINDOWS
/**
 * @def LH_COMPILER_OS_WINDOWS
 * @brief OS identifier for Windows (1).
 */
#    define LH_COMPILER_OS_WINDOWS 1
#endif /* LH_COMPILER_OS_WINDOWS */

#ifndef LH_COMPILER_OS_LINUX
/**
 * @def LH_COMPILER_OS_LINUX
 * @brief OS identifier for Linux (2).
 */
#    define LH_COMPILER_OS_LINUX 2
#endif /* LH_COMPILER_OS_LINUX */

#ifndef LH_COMPILER_OS_MAC
/**
 * @def LH_COMPILER_OS_MAC
 * @brief OS identifier for macOS (3).
 */
#    define LH_COMPILER_OS_MAC 3
#endif /* LH_COMPILER_OS_MAC */

#ifndef LH_COMPILER_OS
/**
 * @def LH_COMPILER_OS
 * @brief Detected target operating system.
 *
 * Set to one of the OS identifier constants based on predefined macros:
 *
 * | Value                      | Detection macros                              |
 * |----------------------------|-----------------------------------------------|
 * | ::LH_COMPILER_OS_WINDOWS   | `_WIN32`, `_WIN64`, `__WIN32__`, `__WINDOWS__`|
 * | ::LH_COMPILER_OS_LINUX     | `__linux__`, `__linux`, `linux`               |
 * | ::LH_COMPILER_OS_MAC       | `__APPLE__`, `__MACH__`                       |
 * | ::LH_COMPILER_OS_UNKNOWN   | *(none of the above)*                         |
 *
 * Can be overridden by defining it before including this header.
 *
 * Example usage:
 * @code{.c}
 * #if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
 *     // Windows-specific code
 * #endif
 * @endcode
 */
#    if defined(_WIN32) || defined(_WIN64) || defined(__WIN32__) || defined(__WINDOWS__)
#        define LH_COMPILER_OS LH_COMPILER_OS_WINDOWS
#    elif defined(__linux__) || defined(__linux) || defined(linux)
#        define LH_COMPILER_OS LH_COMPILER_OS_LINUX
#    elif defined(__APPLE__) || defined(__MACH__)
#        define LH_COMPILER_OS LH_COMPILER_OS_MAC
#    else
#        define LH_COMPILER_OS LH_COMPILER_OS_UNKNOWN
#    endif
#endif /* LH_COMPILER_OS */

#endif /* LH_COMPILER_OS_H */