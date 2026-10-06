/**
 * @file packed.h
 * @brief Rows of small unsigned fields packed high bit first.
 *
 * A row of `count` fields, each `bits` wide (1, 2, 4 or 8), the first field
 * in the high bits of the first byte, padded to a whole byte. LVGL fonts,
 * the fonts `scripts/font.py` writes and the masks `lh/ui/mask.h` reads are
 * laid out this way; naming the read once keeps the shift-and-mask in one
 * place.
 *
 * Every function here is ::LH_ATTRIBUTE_FORCE_INLINE: each is a shift and a
 * mask, cheaper inline than as a call.
 */

#ifndef LH_BIT_PACKED_H
#define LH_BIT_PACKED_H

#include <lh/attribute/force_inline.h>
#include <lh/byte.h>
#include <lh/cast/static.h>
#include <lh/compiler/extern/c.h>
#include <lh/numeric/fixed/types.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Largest value one field of @p bits holds: `2^bits - 1`.
 *
 * @param bits Field width, 1..8.
 */
LH_ATTRIBUTE_FORCE_INLINE
lh_u32_t
lh_bit_packed_max(lh_u32_t bits)
{
    return (1U << bits) - 1U;
}

/**
 * @brief Bytes a row of @p count fields of @p bits takes, padding included.
 */
LH_ATTRIBUTE_FORCE_INLINE
lh_u32_t
lh_bit_packed_bytes(lh_u32_t count, lh_u32_t bits)
{
    return (count * bits + 7U) / 8U;
}

/**
 * @brief Field @p index of @p row, @p bits wide, the first field in the high
 *        bits of `row[0]`.
 *
 * @param row   Packed row (not ::lh_null).
 * @param index Field number from 0.
 * @param bits  Field width: 1, 2, 4 or 8 (a field never spans two bytes).
 */
LH_ATTRIBUTE_FORCE_INLINE
lh_u32_t
lh_bit_packed_get(const lh_byte_t *row, lh_u32_t index, lh_u32_t bits)
{
    const lh_u32_t bit = index * bits;

    return lh_cast_static(lh_u32_t, row[bit / 8U] >> (8U - bits - bit % 8U)) & lh_bit_packed_max(bits);
}

LH_COMPILER_EXTERN_C_END

#endif /* LH_BIT_PACKED_H */
