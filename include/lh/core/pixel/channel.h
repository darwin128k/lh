/**
 * @file channel.h
 * @brief Arithmetic on 8-bit colour channels, one at a time or two in a word.
 *
 * The bottom of every blend: dividing by 255 with rounding and no divide, mixing
 * two channels that share one 32-bit word (`0x00FF00FF` lanes) with one
 * multiply, weighing two colours by their weights, and widening a 5- or 6-bit
 * channel to 8 bits. Nothing here knows a pixel format or a colour type: the
 * words are plain `lh_u32_t`, and every layer above — gl's colour and pixmap,
 * the core's spans — calls these instead of carrying its own copy.
 *
 * Every function is ::LH_ATTRIBUTE_FORCE_INLINE: each is a few integer
 * operations run per pixel, and a real call would cost more than the work.
 */

#ifndef LH_PIXEL_CHANNEL_H
#define LH_PIXEL_CHANNEL_H

#include <lh/core/byte/limits.h>
#include <lh/core/math.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief @p x / 255, rounded to nearest, for @p x up to 255 * 255 — with no divide.
 *
 * 255 is odd, so @p x / 255 never ties at .5, and rounding to nearest is
 * `t = x + 128; (t + t / 256) / 256`. (`(x + 1 + (x >> 8)) >> 8` is the
 * truncating form, not this one.)
 *
 * @param x A product of two channels, or a sum of two such products weighted to 255.
 * @return The quotient, 0..255.
 */
LH_ATTRIBUTE_FORCE_INLINE
lh_u32_t
lh_pixel_channel_div255_round(lh_u32_t x)
{
    const lh_u32_t t = x + 128U;

    return lh_bit_shr(t + lh_bit_shr(t, 8), 8);
}

/**
 * @brief ::lh_pixel_channel_div255_round in each 16-bit lane of @p x at once.
 *
 * A lane holds at most 255 * 255 + 128, so `t + (t >> 8)` stays below 2^16 and
 * never carries into the lane above; the mask drops what the shift brings down
 * from it.
 *
 * @param x Two lane products, in `0x00FF00FF` lanes widened to 16 bits.
 * @return The two quotients, in the `0x00FF00FF` lanes.
 */
LH_ATTRIBUTE_FORCE_INLINE
lh_u32_t
lh_pixel_channel_div255_round_pair(lh_u32_t x)
{
    const lh_u32_t t = x + 0x00800080U;

    return lh_bit_and(lh_bit_shr(t + lh_bit_and(lh_bit_shr(t, 8), 0x00FF00FFU), 8), 0x00FF00FFU);
}

/**
 * @brief Mix two channels at once: @p src over @p dst at alpha @p a, in the
 *        `0x00FF00FF` lanes of one word.
 *
 * One multiply blends both channels, and the products of a lane (255 * 255 at
 * most) never reach the next one.
 *
 * @param dst Two destination channels, in the `0x00FF00FF` lanes.
 * @param src Two source channels, in the same lanes.
 * @param a   Alpha of @p src, 0..255.
 * @return The two mixed channels, in the same lanes.
 */
LH_ATTRIBUTE_FORCE_INLINE
lh_u32_t
lh_pixel_channel_mix_pair(lh_u32_t dst, lh_u32_t src, lh_u32_t a)
{
    return lh_pixel_channel_div255_round_pair(src * a + dst * (LH_BYTE_T_MAX - a));
}

/**
 * @brief One channel of two colours weighed by their weights, rounded.
 *
 * @param src   The source channel, 0..255.
 * @param dst   The destination channel, 0..255.
 * @param src_w The weight of the source; @p src_w + @p dst_w must not be 0.
 * @param dst_w The weight of the destination.
 * @return The weighed channel, 0..255.
 */
LH_ATTRIBUTE_FORCE_INLINE
lh_u32_t
lh_pixel_channel_blend(lh_u32_t src, lh_u32_t dst, lh_u32_t src_w, lh_u32_t dst_w)
{
    return lh_math_div_round(src * src_w + dst * dst_w, src_w + dst_w);
}

/**
 * @brief The alpha a total weight stands for: @p weight / 255, rounded.
 *
 * @param weight A sum of weights, each a product of two 0..255 values.
 * @return The alpha, 0..255.
 */
LH_ATTRIBUTE_FORCE_INLINE
lh_u32_t
lh_pixel_channel_weight_to_alpha(lh_u32_t weight)
{
    return lh_math_div_round(weight, LH_BYTE_T_MAX);
}

/**
 * @brief Widen a @p bits -bit channel to 8 bits by repeating its top bits below:
 *        0 stays 0, all ones becomes 255.
 *
 * @param value The narrow channel.
 * @param bits  Its width, 4..8.
 * @return The channel, 0..255.
 */
LH_ATTRIBUTE_FORCE_INLINE
lh_u32_t
lh_pixel_channel_expand_bits(lh_u32_t value, lh_u32_t bits)
{
    return lh_bit_or(lh_bit_shl(value, 8U - bits), lh_bit_shr(value, 2U * bits - 8U));
}

LH_COMPILER_EXTERN_C_END

#endif /* LH_PIXEL_CHANNEL_H */
