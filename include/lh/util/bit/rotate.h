/**
 * @file rotate.h
 * @brief Rotate the bits of a 32/64-bit word left or right.
 *
 * A rotate is a shift where bits that fall off one end re-enter at the
 * other, so no information is lost — the inverse of a left rotate by @p r
 * is a right rotate by @p r. SHA and Rijndael are built out of these;
 * writing `(x << r) | (x >> (width - r))` by hand at every round is the
 * kind of small, easy-to-get-wrong primitive this library prefers to name
 * once — see `lh/util/bit/scan.h` and `lh/util/bit/endian.h` for the same
 * reasoning applied to bit-scan and endian pack.
 *
 * The shift count is taken modulo the word width, so a rotate by 0 or by
 * the width itself is a no-op and never hits the C shift-by-width
 * undefined behaviour.
 *
 * Every function here is ::LH_ATTRIBUTE_FORCE_INLINE: a rotate is one or
 * two machine instructions, and a real cross-TU call would cost more than
 * the operation itself.
 */

#ifndef LH_UTIL_BIT_ROTATE_H
#define LH_UTIL_BIT_ROTATE_H

#include <lh/attribute/force_inline.h>
#include <lh/compiler/extern/c.h>
#include <lh/compiler/type.h>
#include <lh/numeric/fixed/types.h>
#include <lh/size.h>

#if LH_COMPILER_TYPE == LH_COMPILER_TYPE_MSVC
#    include <stdlib.h>
#endif

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Rotate @p x left by @p r bits (32-bit).
 *
 * @param x Value to rotate.
 * @param r Rotate count; reduced modulo 32.
 * @return @p x rotated left by @p r bits.
 */
LH_ATTRIBUTE_FORCE_INLINE
lh_u32_t
lh_bit_rotate_left_u32(lh_u32_t x, lh_usize_t r)
{
    r &= 31U;
#if LH_COMPILER_TYPE == LH_COMPILER_TYPE_MSVC
    return _rotl(x, (int)r);
#else
    return (r == 0U) ? x : (lh_u32_t)((x << r) | (x >> (32U - r)));
#endif
}

/**
 * @brief Rotate @p x right by @p r bits (32-bit).
 *
 * Inverse of ::lh_bit_rotate_left_u32.
 *
 * @param x Value to rotate.
 * @param r Rotate count; reduced modulo 32.
 * @return @p x rotated right by @p r bits.
 */
LH_ATTRIBUTE_FORCE_INLINE
lh_u32_t
lh_bit_rotate_right_u32(lh_u32_t x, lh_usize_t r)
{
    r &= 31U;
#if LH_COMPILER_TYPE == LH_COMPILER_TYPE_MSVC
    return _rotr(x, (int)r);
#else
    return (r == 0U) ? x : (lh_u32_t)((x >> r) | (x << (32U - r)));
#endif
}

/**
 * @brief Rotate @p x left by @p r bits (64-bit).
 *
 * @param x Value to rotate.
 * @param r Rotate count; reduced modulo 64.
 * @return @p x rotated left by @p r bits.
 */
LH_ATTRIBUTE_FORCE_INLINE
lh_u64_t
lh_bit_rotate_left_u64(lh_u64_t x, lh_usize_t r)
{
    r &= 63U;
#if LH_COMPILER_TYPE == LH_COMPILER_TYPE_MSVC
    return _rotl64(x, (int)r);
#else
    return (r == 0U) ? x : (lh_u64_t)((x << r) | (x >> (64U - r)));
#endif
}

/**
 * @brief Rotate @p x right by @p r bits (64-bit).
 *
 * Inverse of ::lh_bit_rotate_left_u64.
 *
 * @param x Value to rotate.
 * @param r Rotate count; reduced modulo 64.
 * @return @p x rotated right by @p r bits.
 */
LH_ATTRIBUTE_FORCE_INLINE
lh_u64_t
lh_bit_rotate_right_u64(lh_u64_t x, lh_usize_t r)
{
    r &= 63U;
#if LH_COMPILER_TYPE == LH_COMPILER_TYPE_MSVC
    return _rotr64(x, (int)r);
#else
    return (r == 0U) ? x : (lh_u64_t)((x >> r) | (x << (64U - r)));
#endif
}

LH_COMPILER_EXTERN_C_END

#endif /* LH_UTIL_BIT_ROTATE_H */
