/**
 * @file addr.h
 * @brief Pointer-width integers ::lh_uaddr_t / ::lh_saddr_t and related limits.
 *
 * ::LH_COMPILER_ARCH (from `lh/compiler/arch.h`) selects the backing integer width.
 * Auto-detection yields `32` or `64`; for embedded targets you may define
 * `LH_COMPILER_ARCH` to `16` or `8` **before** including `arch.h` to use the
 * corresponding ::lh_u16_t-style range (verify that this matches `sizeof(void*)`
 * on your platform).
 */

#ifndef LH_ADDR_H
#define LH_ADDR_H

#include <lh/compiler/arch.h>
#include <lh/numeric/fixed/limits.h>

#if LH_COMPILER_ARCH == 64

#    ifndef LH_UADDR_T_MIN
/**
 * @def LH_UADDR_T_MIN
 * @brief Minimum value of ::lh_uaddr_t when ::LH_COMPILER_ARCH is `64`.
 */
#        define LH_UADDR_T_MIN LH_U64_T_MIN
#    endif // LH_UADDR_T_MIN

#    ifndef LH_UADDR_T_MAX
/**
 * @def LH_UADDR_T_MAX
 * @brief Maximum value of ::lh_uaddr_t when ::LH_COMPILER_ARCH is `64`.
 */
#        define LH_UADDR_T_MAX LH_U64_T_MAX
#    endif // LH_UADDR_T_MAX

/**
 * @typedef lh_uaddr_t
 * @brief Unsigned integer wide enough for pointer-sized storage (`LH_COMPILER_ARCH == 64`).
 */
typedef lh_ullong_t lh_uaddr_t;

#    ifndef LH_SADDR_T_MIN
/**
 * @def LH_SADDR_T_MIN
 * @brief Minimum value of ::lh_saddr_t when ::LH_COMPILER_ARCH is `64`.
 */
#        define LH_SADDR_T_MIN LH_S64_T_MIN
#    endif // LH_SADDR_T_MIN

#    ifndef LH_SADDR_T_MAX
/**
 * @def LH_SADDR_T_MAX
 * @brief Maximum value of ::lh_saddr_t when ::LH_COMPILER_ARCH is `64`.
 */
#        define LH_SADDR_T_MAX LH_S64_T_MAX
#    endif // LH_SADDR_T_MAX

/**
 * @typedef lh_saddr_t
 * @brief Signed integer wide enough for pointer-sized storage (`LH_COMPILER_ARCH == 64`).
 */
typedef lh_sllong_t lh_saddr_t;

#elif LH_COMPILER_ARCH == 32

#    ifndef LH_UADDR_T_MIN
/**
 * @def LH_UADDR_T_MIN
 * @brief Minimum value of ::lh_uaddr_t when ::LH_COMPILER_ARCH is `32`.
 */
#        define LH_UADDR_T_MIN LH_U32_T_MIN
#    endif // LH_UADDR_T_MIN

#    ifndef LH_UADDR_T_MAX
/**
 * @def LH_UADDR_T_MAX
 * @brief Maximum value of ::lh_uaddr_t when ::LH_COMPILER_ARCH is `32`.
 */
#        define LH_UADDR_T_MAX LH_U32_T_MAX
#    endif // LH_UADDR_T_MAX

/**
 * @typedef lh_uaddr_t
 * @brief Unsigned integer wide enough for pointer-sized storage (`LH_COMPILER_ARCH == 32`).
 */
typedef lh_uint_t lh_uaddr_t;

#    ifndef LH_SADDR_T_MIN
/**
 * @def LH_SADDR_T_MIN
 * @brief Minimum value of ::lh_saddr_t when ::LH_COMPILER_ARCH is `32`.
 */
#        define LH_SADDR_T_MIN LH_S32_T_MIN
#    endif // LH_SADDR_T_MIN

#    ifndef LH_SADDR_T_MAX
/**
 * @def LH_SADDR_T_MAX
 * @brief Maximum value of ::lh_saddr_t when ::LH_COMPILER_ARCH is `32`.
 */
#        define LH_SADDR_T_MAX LH_S32_T_MAX
#    endif // LH_SADDR_T_MAX

/**
 * @typedef lh_saddr_t
 * @brief Signed integer wide enough for pointer-sized storage (`LH_COMPILER_ARCH == 32`).
 */
typedef lh_sint_t lh_saddr_t;

#elif LH_COMPILER_ARCH == 16

#    ifndef LH_UADDR_T_MIN
/**
 * @def LH_UADDR_T_MIN
 * @brief Minimum value of ::lh_uaddr_t when ::LH_COMPILER_ARCH is `16` (manual config).
 */
#        define LH_UADDR_T_MIN LH_U16_T_MIN
#    endif // LH_UADDR_T_MIN

#    ifndef LH_UADDR_T_MAX
/**
 * @def LH_UADDR_T_MAX
 * @brief Maximum value of ::lh_uaddr_t when ::LH_COMPILER_ARCH is `16` (manual config).
 */
#        define LH_UADDR_T_MAX LH_U16_T_MAX
#    endif // LH_UADDR_T_MAX

/**
 * @typedef lh_uaddr_t
 * @brief Unsigned 16-bit address storage (`LH_COMPILER_ARCH == 16`, manual).
 */
typedef lh_ushort_t lh_uaddr_t;

#    ifndef LH_SADDR_T_MIN
/**
 * @def LH_SADDR_T_MIN
 * @brief Minimum value of ::lh_saddr_t when ::LH_COMPILER_ARCH is `16` (manual config).
 */
#        define LH_SADDR_T_MIN LH_S16_T_MIN
#    endif // LH_SADDR_T_MIN

#    ifndef LH_SADDR_T_MAX
/**
 * @def LH_SADDR_T_MAX
 * @brief Maximum value of ::lh_saddr_t when ::LH_COMPILER_ARCH is `16` (manual config).
 */
#        define LH_SADDR_T_MAX LH_S16_T_MAX
#    endif // LH_SADDR_T_MAX

/**
 * @typedef lh_saddr_t
 * @brief Signed 16-bit address storage (`LH_COMPILER_ARCH == 16`, manual).
 */
typedef lh_sshort_t lh_saddr_t;

#elif LH_COMPILER_ARCH == 8

#    ifndef LH_UADDR_T_MIN
/**
 * @def LH_UADDR_T_MIN
 * @brief Minimum value of ::lh_uaddr_t when ::LH_COMPILER_ARCH is `8` (manual config).
 */
#        define LH_UADDR_T_MIN LH_U8_T_MIN
#    endif // LH_UADDR_T_MIN

#    ifndef LH_UADDR_T_MAX
/**
 * @def LH_UADDR_T_MAX
 * @brief Maximum value of ::lh_uaddr_t when ::LH_COMPILER_ARCH is `8` (manual config).
 */
#        define LH_UADDR_T_MAX LH_U8_T_MAX
#    endif // LH_UADDR_T_MAX

/**
 * @typedef lh_uaddr_t
 * @brief Unsigned 8-bit address storage (`LH_COMPILER_ARCH == 8`, manual).
 */
typedef lh_uchar_t lh_uaddr_t;

#    ifndef LH_SADDR_T_MIN
/**
 * @def LH_SADDR_T_MIN
 * @brief Minimum value of ::lh_saddr_t when ::LH_COMPILER_ARCH is `8` (manual config).
 */
#        define LH_SADDR_T_MIN LH_S8_T_MIN
#    endif // LH_SADDR_T_MIN

#    ifndef LH_SADDR_T_MAX
/**
 * @def LH_SADDR_T_MAX
 * @brief Maximum value of ::lh_saddr_t when ::LH_COMPILER_ARCH is `8` (manual config).
 */
#        define LH_SADDR_T_MAX LH_S8_T_MAX
#    endif // LH_SADDR_T_MAX

/**
 * @typedef lh_saddr_t
 * @brief Signed 8-bit address storage (`LH_COMPILER_ARCH == 8`, manual).
 */
typedef lh_schar_t lh_saddr_t;

#else

#    error "LH_COMPILER_ARCH must be 8, 16, 32, or 64 for lh/addr.h"

#endif

#endif // LH_ADDR_H
