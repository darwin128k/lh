/**
 * @file addr.h
 * @brief Pointer-width integers ::lh_uaddr_t / ::lh_saddr_t.
 *
 * ::LH_COMPILER_ARCH (from `lh/compiler/arch.h`) selects the backing integer width.
 * Auto-detection yields `32` or `64`; for embedded targets you may define
 * `LH_COMPILER_ARCH` to `16` or `8` **before** including `arch.h` to use the
 * corresponding ::lh_u16_t-style range (verify that this matches `sizeof(void*)`
 * on your platform).
 *
 * Limits (min/max/size macros) live in <lh/addr/limits.h>.
 */

#ifndef LH_ADDR_H
#define LH_ADDR_H

#include <lh/byte.h>
#include <lh/compiler/arch.h>

#if LH_COMPILER_ARCH == LH_COMPILER_ARCH_64

/**
 * @typedef lh_uaddr_t
 * @brief Unsigned integer wide enough for pointer-sized storage (`LH_COMPILER_ARCH == 64`).
 */
typedef lh_ullong_t lh_uaddr_t;

/**
 * @typedef lh_saddr_t
 * @brief Signed integer wide enough for pointer-sized storage (`LH_COMPILER_ARCH == 64`).
 */
typedef lh_sllong_t lh_saddr_t;

#elif LH_COMPILER_ARCH == LH_COMPILER_ARCH_32

/**
 * @typedef lh_uaddr_t
 * @brief Unsigned integer wide enough for pointer-sized storage (`LH_COMPILER_ARCH == 32`).
 */
typedef lh_uint_t lh_uaddr_t;

/**
 * @typedef lh_saddr_t
 * @brief Signed integer wide enough for pointer-sized storage (`LH_COMPILER_ARCH == 32`).
 */
typedef lh_sint_t lh_saddr_t;

#elif LH_COMPILER_ARCH == LH_COMPILER_ARCH_16

/**
 * @typedef lh_uaddr_t
 * @brief Unsigned 16-bit address storage (`LH_COMPILER_ARCH == 16`, manual).
 */
typedef lh_ushort_t lh_uaddr_t;

/**
 * @typedef lh_saddr_t
 * @brief Signed 16-bit address storage (`LH_COMPILER_ARCH == 16`, manual).
 */
typedef lh_sshort_t lh_saddr_t;

#elif LH_COMPILER_ARCH == LH_COMPILER_ARCH_8

/**
 * @typedef lh_uaddr_t
 * @brief Unsigned 8-bit address storage (`LH_COMPILER_ARCH == 8`, manual).
 */
typedef lh_byte_t lh_uaddr_t;

/**
 * @typedef lh_saddr_t
 * @brief Signed 8-bit address storage (`LH_COMPILER_ARCH == 8`, manual).
 */
typedef lh_schar_t lh_saddr_t;

#else

#    error "LH_COMPILER_ARCH must be 8, 16, 32, or 64 for lh/addr.h"

#endif

#endif /* LH_ADDR_H */
