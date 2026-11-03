/**
 * @file reverse.h
 * @brief Reverse the order of the low bits of a 32-bit word.
 *
 * Formats that store bit fields low-bit-first but define their codes
 * high-bit-first — DEFLATE's canonical Huffman codes are the main example —
 * need a code mirrored within its own width before it is written. The width
 * is an argument, so a 5-bit distance code and a 9-bit literal code use the
 * same brick.
 *
 * The function is ::LH_ATTRIBUTE_FORCE_INLINE, like the other `lh/bit`
 * primitives (`lh/bit/rotate.h`, `lh/bit/scan.h`): it runs once per emitted
 * code while a table is built, and a cross-TU call would cost more than it.
 */

#ifndef LH_BIT_REVERSE_H
#define LH_BIT_REVERSE_H

#include <lh/attribute/force_inline.h>
#include <lh/bit/ops.h>
#include <lh/compiler/extern/c.h>
#include <lh/numeric/fixed/types.h>
#include <lh/size.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Mirror the low @p count bits of @p x; higher bits are dropped.
 *
 * Bit 0 of the result is bit `count - 1` of @p x, bit 1 is bit `count - 2`,
 * and so on. A count of zero returns zero.
 *
 * @param x Value whose low bits are reversed.
 * @param count Number of low bits to reverse, at most 32.
 * @return The reversed bits in the low @p count bits; all higher bits are zero.
 * @pre @p count is at most 32; larger values are not checked.
 */
LH_ATTRIBUTE_FORCE_INLINE
lh_u32_t
lh_bit_reverse_u32(lh_u32_t x, lh_usize_t count)
{
    lh_u32_t result = 0U;
    for (lh_usize_t i = 0U; lh_math_lt(i, count); i = lh_math_add_one(i))
    {
        result = lh_bit_or(lh_bit_shl(result, 1U), lh_bit_and(x, 1U));
        x = lh_bit_shr(x, 1U);
    }
    return result;
}

LH_COMPILER_EXTERN_C_END

#endif /* LH_BIT_REVERSE_H */
