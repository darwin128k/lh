/**
 * @file bswap.h
 * @brief Reverse the bytes of a 16/32/64-bit integer in register.
 *
 * Distinct from `lh/util/bit/endian.h`: pack/unpack writes a value into a
 * byte buffer in a chosen order, while a bswap rearranges the bytes of the
 * integer itself. SHA and Rijndael (and `lh/memory/std.c`'s reverse-copy
 * path) need the in-register form.
 *
 * Portable wrapper over `__builtin_bswap{16,32,64}` (GCC/Clang) and
 * `_byteswap_{ushort,ulong,uint64}` (MSVC). A portable shift fallback is
 * used for any other compiler.
 *
 * Every function here is ::LH_ATTRIBUTE_FORCE_INLINE: a bswap is one
 * instruction on every target this library cares about, and a real
 * cross-TU call would cost more than the operation itself.
 */

#ifndef LH_UTIL_BIT_BSWAP_H
#define LH_UTIL_BIT_BSWAP_H

#include <lh/attribute/force_inline.h>
#include <lh/cast/static.h>
#include <lh/compiler/extern/c.h>
#include <lh/compiler/type.h>
#include <lh/numeric/fixed/types.h>

#if LH_COMPILER_TYPE == LH_COMPILER_TYPE_MSVC
#    include <stdlib.h>
#endif

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Reverse the two bytes of @p x.
 *
 * @param x Value to swap.
 * @return @p x with its bytes reversed.
 */
LH_ATTRIBUTE_FORCE_INLINE
lh_u16_t
lh_bit_bswap_u16(lh_u16_t x)
{
#if LH_COMPILER_TYPE_IS_GCC_LIKE
    return __builtin_bswap16(x);
#elif LH_COMPILER_TYPE == LH_COMPILER_TYPE_MSVC
    return _byteswap_ushort(x);
#else
    return lh_cast_static(lh_u16_t, (lh_u16_t)((x << 8) | (x >> 8)));
#endif
}

/**
 * @brief Reverse the four bytes of @p x.
 *
 * @param x Value to swap.
 * @return @p x with its bytes reversed.
 */
LH_ATTRIBUTE_FORCE_INLINE
lh_u32_t
lh_bit_bswap_u32(lh_u32_t x)
{
#if LH_COMPILER_TYPE_IS_GCC_LIKE
    return __builtin_bswap32(x);
#elif LH_COMPILER_TYPE == LH_COMPILER_TYPE_MSVC
    return _byteswap_ulong(x);
#else
    return (x << 24) | ((x << 8) & 0x00FF0000U) | ((x >> 8) & 0x0000FF00U) | (x >> 24);
#endif
}

/**
 * @brief Reverse the eight bytes of @p x.
 *
 * @param x Value to swap.
 * @return @p x with its bytes reversed.
 */
LH_ATTRIBUTE_FORCE_INLINE
lh_u64_t
lh_bit_bswap_u64(lh_u64_t x)
{
#if LH_COMPILER_TYPE_IS_GCC_LIKE
    return __builtin_bswap64(x);
#elif LH_COMPILER_TYPE == LH_COMPILER_TYPE_MSVC
    return _byteswap_uint64(x);
#else
    return (lh_u64_t)lh_bit_bswap_u32((lh_u32_t)x) << 32 | lh_bit_bswap_u32((lh_u32_t)(x >> 32));
#endif
}

LH_COMPILER_EXTERN_C_END

#endif /* LH_UTIL_BIT_BSWAP_H */
