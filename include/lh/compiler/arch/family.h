/**
 * @file family.h
 * @brief Target instruction set family detection (x86, ARM, ...).
 *
 * Provides ::LH_COMPILER_ARCH_FAMILY — which instruction set family is
 * targeted (x86, ARM, or unknown) — and the derived predicates
 * ::LH_COMPILER_ARCH_FAMILY_IS_X86 / ::LH_COMPILER_ARCH_FAMILY_IS_ARM, for
 * guarding family-specific code such as x86 intrinsics headers.
 */

#ifndef LH_COMPILER_ARCH_FAMILY_H
#define LH_COMPILER_ARCH_FAMILY_H

#ifndef LH_COMPILER_ARCH_FAMILY_UNKNOWN
/**
 * @def LH_COMPILER_ARCH_FAMILY_UNKNOWN
 * @brief Architecture family identifier for an unrecognised family (0).
 */
#    define LH_COMPILER_ARCH_FAMILY_UNKNOWN 0
#endif /* LH_COMPILER_ARCH_FAMILY_UNKNOWN */

#ifndef LH_COMPILER_ARCH_FAMILY_X86
/**
 * @def LH_COMPILER_ARCH_FAMILY_X86
 * @brief Architecture family identifier for x86 / x86-64 (1).
 */
#    define LH_COMPILER_ARCH_FAMILY_X86 1
#endif /* LH_COMPILER_ARCH_FAMILY_X86 */

#ifndef LH_COMPILER_ARCH_FAMILY_ARM
/**
 * @def LH_COMPILER_ARCH_FAMILY_ARM
 * @brief Architecture family identifier for ARM / AArch64 (2).
 */
#    define LH_COMPILER_ARCH_FAMILY_ARM 2
#endif /* LH_COMPILER_ARCH_FAMILY_ARM */

#ifndef LH_COMPILER_ARCH_FAMILY
/**
 * @def LH_COMPILER_ARCH_FAMILY
 * @brief Detected instruction set family.
 *
 * Set to one of the family identifier constants based on predefined macros.
 * Can be overridden by defining it before including this header.
 *
 * Example usage:
 * @code{.c}
 * #if LH_COMPILER_ARCH_FAMILY == LH_COMPILER_ARCH_FAMILY_X86
 *     // x86-specific code
 * #endif
 * @endcode
 */
#    if defined(__x86_64__) || defined(__x86_64) || defined(__amd64__) || defined(__amd64) ||        \
        defined(_M_X64) || defined(_M_IX86) || defined(__i386__) || defined(__i386) ||               \
        defined(i386)
#        define LH_COMPILER_ARCH_FAMILY LH_COMPILER_ARCH_FAMILY_X86
#    elif defined(__aarch64__) || defined(__arm64__) || defined(_M_ARM64) || defined(__arm__) ||     \
        defined(_M_ARM) || defined(__ARM_ARCH)
#        define LH_COMPILER_ARCH_FAMILY LH_COMPILER_ARCH_FAMILY_ARM
#    else
#        define LH_COMPILER_ARCH_FAMILY LH_COMPILER_ARCH_FAMILY_UNKNOWN
#    endif
#endif /* LH_COMPILER_ARCH_FAMILY */

#ifndef LH_COMPILER_ARCH_FAMILY_IS_X86
/**
 * @def LH_COMPILER_ARCH_FAMILY_IS_X86
 * @brief Predicate: non-zero when ::LH_COMPILER_ARCH_FAMILY is x86 / x86-64.
 *
 * Use in `#if` directives to guard x86-specific intrinsics (e.g. `<immintrin.h>`,
 * `<intrin.h>`).
 */
#    define LH_COMPILER_ARCH_FAMILY_IS_X86                                                           \
        (LH_COMPILER_ARCH_FAMILY == LH_COMPILER_ARCH_FAMILY_X86)
#endif /* LH_COMPILER_ARCH_FAMILY_IS_X86 */

#ifndef LH_COMPILER_ARCH_FAMILY_IS_ARM
/**
 * @def LH_COMPILER_ARCH_FAMILY_IS_ARM
 * @brief Predicate: non-zero when ::LH_COMPILER_ARCH_FAMILY is ARM / AArch64.
 *
 * Use in `#if` directives to guard ARM-specific intrinsics (e.g. `<arm_neon.h>`).
 */
#    define LH_COMPILER_ARCH_FAMILY_IS_ARM                                                           \
        (LH_COMPILER_ARCH_FAMILY == LH_COMPILER_ARCH_FAMILY_ARM)
#endif /* LH_COMPILER_ARCH_FAMILY_IS_ARM */

#endif /* LH_COMPILER_ARCH_FAMILY_H */
