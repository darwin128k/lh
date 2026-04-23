/**
 * @file type.h
 * @brief Compiler identification macros.
 *
 * Provides compiler identifier constants (::LH_COMPILER_TYPE_UNKNOWN,
 * ::LH_COMPILER_TYPE_GCC, ::LH_COMPILER_TYPE_MSVC, ::LH_COMPILER_TYPE_CLANG),
 * ::LH_COMPILER_TYPE — a compile-time constant set to the detected compiler —
 * and ::LH_COMPILER_TYPE_IS_GCC_LIKE — a predicate for GCC-compatible compilers.
 * All macros can be overridden by defining them before including this header.
 */

#ifndef LH_COMPILER_TYPE_H
#define LH_COMPILER_TYPE_H

#ifndef LH_COMPILER_TYPE_UNKNOWN
/**
 * @def LH_COMPILER_TYPE_UNKNOWN
 * @brief Compiler identifier for an unrecognised compiler (0).
 */
#    define LH_COMPILER_TYPE_UNKNOWN 0
#endif // LH_COMPILER_TYPE_UNKNOWN

#ifndef LH_COMPILER_TYPE_GCC
/**
 * @def LH_COMPILER_TYPE_GCC
 * @brief Compiler identifier for GCC (1).
 */
#    define LH_COMPILER_TYPE_GCC 1
#endif // LH_COMPILER_TYPE_GCC

#ifndef LH_COMPILER_TYPE_MSVC
/**
 * @def LH_COMPILER_TYPE_MSVC
 * @brief Compiler identifier for MSVC (2).
 */
#    define LH_COMPILER_TYPE_MSVC 2
#endif // LH_COMPILER_TYPE_MSVC

#ifndef LH_COMPILER_TYPE_CLANG
/**
 * @def LH_COMPILER_TYPE_CLANG
 * @brief Compiler identifier for Clang (3).
 */
#    define LH_COMPILER_TYPE_CLANG 3
#endif // LH_COMPILER_TYPE_CLANG

/**
 * @def LH_COMPILER_TYPE
 * @brief Detected compiler.
 *
 * Set to one of the compiler identifier constants based on predefined macros:
 *
 * | Value                          | Detection macro       |
 * |--------------------------------|-----------------------|
 * | ::LH_COMPILER_TYPE_CLANG       | `__clang__`           |
 * | ::LH_COMPILER_TYPE_MSVC        | `_MSC_VER`            |
 * | ::LH_COMPILER_TYPE_GCC         | `__GNUC__`            |
 * | ::LH_COMPILER_TYPE_UNKNOWN     | *(none of the above)* |
 *
 * Clang is tested before GCC because Clang also defines `__GNUC__`.
 * Can be overridden by defining it before including this header.
 *
 * Example usage:
 * @code{.c}
 * #if LH_COMPILER_TYPE == LH_COMPILER_TYPE_MSVC
 *     // MSVC-specific code
 * #endif
 * @endcode
 */
#if defined(__clang__)
#    ifndef LH_COMPILER_TYPE
#        define LH_COMPILER_TYPE LH_COMPILER_TYPE_CLANG
#    endif // LH_COMPILER_TYPE

#elif defined(_MSC_VER)
#    ifndef LH_COMPILER_TYPE
#        define LH_COMPILER_TYPE LH_COMPILER_TYPE_MSVC
#    endif // LH_COMPILER_TYPE

#elif defined(__GNUC__)
#    ifndef LH_COMPILER_TYPE
#        define LH_COMPILER_TYPE LH_COMPILER_TYPE_GCC
#    endif // LH_COMPILER_TYPE

#else
#    ifndef LH_COMPILER_TYPE
#        define LH_COMPILER_TYPE LH_COMPILER_TYPE_UNKNOWN
#    endif // LH_COMPILER_TYPE
#endif

#ifndef LH_COMPILER_TYPE_IS_GCC_LIKE
/**
 * @def LH_COMPILER_TYPE_IS_GCC_LIKE
 * @brief Predicate: non-zero when the compiler is GCC or Clang.
 *
 * Evaluates to a non-zero constant expression when ::LH_COMPILER_TYPE is
 * ::LH_COMPILER_TYPE_GCC or ::LH_COMPILER_TYPE_CLANG, zero otherwise.
 * Use in `#if` directives to guard GCC/Clang-specific extensions such as
 * `__attribute__((...))`.
 *
 * Example usage:
 * @code{.c}
 * #if LH_COMPILER_TYPE_IS_GCC_LIKE
 *     // GCC / Clang specific code
 * #endif
 * @endcode
 */
#    define LH_COMPILER_TYPE_IS_GCC_LIKE                                                           \
        (LH_COMPILER_TYPE == LH_COMPILER_TYPE_GCC) || (LH_COMPILER_TYPE == LH_COMPILER_TYPE_CLANG)
#endif // LH_COMPILER_TYPE_IS_GCC_LIKE

#endif // LH_COMPILER_TYPE_H