/**
 * @file argb.h
 * @brief A colour as one 32-bit word `0xAARRGGBB`, and laying one over another.
 *
 * The word is straight alpha (not premultiplied), the same layout as a 32-bit
 * Win32 DIB on a little-endian target. Laying a colour over a pixel comes in two
 * kinds, because the cheap one is the common one: over an **opaque** pixel the
 * result is opaque, and red/blue and green go through one multiply pair each;
 * over a **translucent** pixel the alphas have to be weighed too.
 *
 * Every function is ::LH_ATTRIBUTE_FORCE_INLINE: they run per pixel.
 */

#ifndef LH_PIXEL_ARGB_H
#define LH_PIXEL_ARGB_H

#include <lh/core/pixel/channel.h>
#include <lh/core/util/return.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The word of a colour from its four channels.
 *
 * @param r Red, 0..255.
 * @param g Green, 0..255.
 * @param b Blue, 0..255.
 * @param a Alpha, 0..255.
 * @return `0xAARRGGBB`.
 */
LH_ATTRIBUTE_FORCE_INLINE
lh_u32_t
lh_pixel_argb_make(lh_u32_t r, lh_u32_t g, lh_u32_t b, lh_u32_t a)
{
    return lh_bit_or(lh_bit_or(lh_bit_shl(a, 24), lh_bit_shl(r, 16)),
                     lh_bit_or(lh_bit_shl(g, 8), b));
}

/**
 * @brief The channel of @p argb that starts at bit @p shift.
 *
 * @param argb  The word.
 * @param shift 24 for alpha, 16 for red, 8 for green, 0 for blue.
 * @return The channel, 0..255.
 */
LH_ATTRIBUTE_FORCE_INLINE
lh_u32_t
lh_pixel_argb_get_channel(lh_u32_t argb, lh_u32_t shift)
{
    return lh_bit_and(lh_bit_shr(argb, shift), lh_cast_static(lh_u32_t, LH_BYTE_T_MAX));
}

/**
 * @brief @p src laid over an opaque @p dst.
 *
 * Red and blue in one word, green (and the alpha lane, which an opaque result
 * does not read) in another: two multiplies a pair instead of one a channel.
 *
 * @param dst The pixel under, whose alpha is taken as 255.
 * @param src The colour over, with its alpha.
 * @return The opaque result.
 */
LH_ATTRIBUTE_FORCE_INLINE
lh_u32_t
lh_pixel_argb_over_opaque(lh_u32_t dst, lh_u32_t src)
{
    const lh_u32_t a = lh_pixel_argb_get_channel(src, 24);
    const lh_u32_t rb =
        lh_pixel_channel_mix_pair(lh_bit_and(dst, 0x00FF00FFU), lh_bit_and(src, 0x00FF00FFU), a);
    const lh_u32_t ga = lh_pixel_channel_mix_pair(lh_bit_and(lh_bit_shr(dst, 8), 0x00FF00FFU),
                                                  lh_bit_and(lh_bit_shr(src, 8), 0x00FF00FFU), a);

    return lh_bit_or(lh_bit_or(0xFF000000U, rb), lh_bit_shl(lh_bit_and(ga, 0xFFU), 8));
}

/**
 * @brief @p src laid over a @p dst that may be translucent: the "over" operator
 *        on straight alpha.
 *
 * @param dst The pixel under, with its alpha.
 * @param src The colour over, with its alpha.
 * @return The result; `0` when both are fully transparent.
 */
LH_ATTRIBUTE_FORCE_INLINE
lh_u32_t
lh_pixel_argb_over_translucent(lh_u32_t dst, lh_u32_t src)
{
    const lh_u32_t a = lh_pixel_argb_get_channel(src, 24);
    const lh_u32_t src_w = a * LH_BYTE_T_MAX;
    const lh_u32_t dst_w = lh_pixel_argb_get_channel(dst, 24) * (LH_BYTE_T_MAX - a);

    lh_return_if(src_w + dst_w == 0U, 0U);
    return lh_pixel_argb_make(
        lh_pixel_channel_blend(lh_pixel_argb_get_channel(src, 16),
                               lh_pixel_argb_get_channel(dst, 16), src_w, dst_w),
        lh_pixel_channel_blend(lh_pixel_argb_get_channel(src, 8), lh_pixel_argb_get_channel(dst, 8),
                               src_w, dst_w),
        lh_pixel_channel_blend(lh_pixel_argb_get_channel(src, 0), lh_pixel_argb_get_channel(dst, 0),
                               src_w, dst_w),
        lh_pixel_channel_weight_to_alpha(src_w + dst_w));
}

/**
 * @brief @p src laid over @p dst, taking the cheap path when @p dst is opaque.
 *
 * @param dst The pixel under.
 * @param src The colour over.
 * @return The result.
 */
LH_ATTRIBUTE_FORCE_INLINE
lh_u32_t
lh_pixel_argb_over(lh_u32_t dst, lh_u32_t src)
{
    return lh_pixel_argb_get_channel(dst, 24) == LH_BYTE_T_MAX
               ? lh_pixel_argb_over_opaque(dst, src)
               : lh_pixel_argb_over_translucent(dst, src);
}

LH_COMPILER_EXTERN_C_END

#endif /* LH_PIXEL_ARGB_H */
