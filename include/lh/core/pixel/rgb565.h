/**
 * @file rgb565.h
 * @brief A colour as one 16-bit word `rrrrrggg gggbbbbb`, and its link to `0xAARRGGBB`.
 *
 * The word is in native order and opaque; a display that takes the two bytes
 * swapped swaps them on the way (::lh_bit_bswap_u16). Widening repeats the top
 * bits of a channel below it (0 stays 0, all ones becomes 255); narrowing is
 * truncation. The blend works on the widened word with the same lane trick as
 * ::lh_pixel_argb_over_opaque, so a 565 blend gives, bit for bit, the 888 blend
 * narrowed — the path an MCU runs.
 *
 * Every function is ::LH_ATTRIBUTE_FORCE_INLINE: they run per pixel.
 */

#ifndef LH_PIXEL_RGB565_H
#define LH_PIXEL_RGB565_H

#include <lh/core/bit/bswap.h>
#include <lh/core/pixel/argb.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Red and blue of @p word widened to 8 bits, in the `0x00FF00FF` lanes.
 *
 * Red goes to bits 16.., blue stays at 0; `v * 33 >> 2` is `(v << 3) | (v >> 2)`
 * for a 5-bit `v`, and the mask drops what the shift brings down from the red
 * lane.
 *
 * @param word The 565 word, native order.
 * @return `0x00RR00BB`.
 */
LH_ATTRIBUTE_FORCE_INLINE
lh_u32_t
lh_pixel_rgb565_get_rb(lh_u32_t word)
{
    return lh_bit_and(
        lh_bit_shr(lh_bit_or(lh_bit_shl(lh_bit_and(word, 0xF800U), 5), lh_bit_and(word, 0x001FU)) *
                       33U,
                   2),
        0x00FF00FFU);
}

/**
 * @brief Green of @p word widened to 8 bits.
 *
 * `g * 65 >> 4` is `(g << 2) | (g >> 4)` for a 6-bit `g`.
 *
 * @param word The 565 word, native order.
 * @return Green, 0..255.
 */
LH_ATTRIBUTE_FORCE_INLINE
lh_u32_t
lh_pixel_rgb565_get_g(lh_u32_t word)
{
    return lh_bit_shr(lh_bit_and(lh_bit_shr(word, 5), 0x3FU) * 65U, 4);
}

/**
 * @brief The 565 word of red and blue in the `0x00FF00FF` lanes and @p g,
 *        narrowed by truncation.
 *
 * @param rb `0x00RR00BB`.
 * @param g  Green, 0..255.
 * @return The 565 word, native order.
 */
LH_ATTRIBUTE_FORCE_INLINE
lh_u32_t
lh_pixel_rgb565_make(lh_u32_t rb, lh_u32_t g)
{
    return lh_bit_or(lh_bit_or(lh_bit_shl(lh_bit_shr(rb, 19), 11), lh_bit_shl(lh_bit_shr(g, 2), 5)),
                     lh_bit_shr(lh_bit_and(rb, 0xFFU), 3));
}

/**
 * @brief The 565 word of @p argb, its alpha dropped.
 *
 * @param argb `0xAARRGGBB`.
 * @return The 565 word, native order.
 */
LH_ATTRIBUTE_FORCE_INLINE
lh_u32_t
lh_pixel_rgb565_from_argb(lh_u32_t argb)
{
    return lh_pixel_rgb565_make(lh_bit_and(argb, 0x00FF00FFU), lh_pixel_argb_get_channel(argb, 8));
}

/**
 * @brief The opaque `0xAARRGGBB` of @p word.
 *
 * @param word The 565 word, native order.
 * @return `0xFFRRGGBB`.
 */
LH_ATTRIBUTE_FORCE_INLINE
lh_u32_t
lh_pixel_rgb565_to_argb(lh_u32_t word)
{
    return lh_bit_or(lh_bit_or(0xFF000000U, lh_pixel_rgb565_get_rb(word)),
                     lh_bit_shl(lh_pixel_rgb565_get_g(word), 8));
}

/**
 * @brief @p word in the byte order of the memory: as it is, or with its two bytes
 *        swapped for a display that takes them big-endian.
 *
 * The same call turns a stored word into native order and back.
 *
 * @param word    The 565 word.
 * @param swapped True when the memory holds the bytes swapped.
 * @return The word in the other order when @p swapped, else @p word.
 */
LH_ATTRIBUTE_FORCE_INLINE
lh_u32_t
lh_pixel_rgb565_order(lh_u32_t word, lh_bool_t swapped)
{
    return swapped ? lh_bit_bswap_u16(lh_cast_static(lh_u16_t, word)) : word;
}

/**
 * @brief @p src laid over the 565 pixel @p word at alpha @p a.
 *
 * The 888 blend of the widened word, bit for bit, without unpacking a channel at
 * a time: red and blue go through one multiply pair, green through the other.
 *
 * @param word The pixel under, 565, native order.
 * @param src  The colour over, `0x..RRGGBB` (its alpha is ignored).
 * @param a    The alpha to lay it with, 0..255.
 * @return The 565 result, native order.
 */
LH_ATTRIBUTE_FORCE_INLINE
lh_u32_t
lh_pixel_rgb565_mix(lh_u32_t word, lh_u32_t src, lh_u32_t a)
{
    return lh_pixel_rgb565_make(
        lh_pixel_channel_mix_pair(lh_pixel_rgb565_get_rb(word), lh_bit_and(src, 0x00FF00FFU), a),
        lh_pixel_channel_mix_pair(lh_pixel_rgb565_get_g(word),
                                  lh_bit_and(lh_bit_shr(src, 8), 0xFFU), a));
}

LH_COMPILER_EXTERN_C_END

#endif /* LH_PIXEL_RGB565_H */
