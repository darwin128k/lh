/**
 * @file span.c
 * @brief Implementation of `lh/core/pixel/span.h`.
 */

#include <lh/core/pixel/span.h>

#include <lh/core/memory.h>
#include <lh/core/pixel/rgb565.h>

lh_void
lh_pixel_span_blend_argb(lh_u32_t *at, lh_usize_t count, lh_u32_t src, lh_u32_t alpha)
{
    lh_return_if(count == 0U || alpha == 0U);
    if (alpha == LH_BYTE_T_MAX)
    {
        lh_memory_set_u32(at, count, lh_bit_or(0xFF000000U, src));
        return;
    }
    const lh_u32_t over = lh_bit_or(lh_bit_shl(alpha, 24), lh_bit_and(src, 0x00FFFFFFU));

    for (lh_usize_t i = 0U; i < count;)
    {
        const lh_u32_t dst = at[i];
        lh_usize_t end = i + 1U;

        while (end < count && at[end] == dst)
        {
            ++end;
        }
        lh_memory_set_u32(at + i, end - i, lh_pixel_argb_over(dst, over));
        i = end;
    }
}

lh_void
lh_pixel_span_blend_rgb565(lh_u16_t *at, lh_usize_t count, lh_u32_t src, lh_u32_t alpha,
                           lh_bool_t swapped)
{
    lh_return_if(count == 0U || alpha == 0U);
    if (alpha == LH_BYTE_T_MAX)
    {
        lh_memory_set_u16(at, count,
                          lh_cast_static(lh_u16_t, lh_pixel_rgb565_order(
                                                       lh_pixel_rgb565_from_argb(src), swapped)));
        return;
    }
    for (lh_usize_t i = 0U; i < count;)
    {
        const lh_u16_t dst = at[i];
        lh_usize_t end = i + 1U;

        while (end < count && at[end] == dst)
        {
            ++end;
        }
        const lh_u32_t mixed = lh_pixel_rgb565_mix(lh_pixel_rgb565_order(dst, swapped), src, alpha);

        lh_memory_set_u16(at + i, end - i,
                          lh_cast_static(lh_u16_t, lh_pixel_rgb565_order(mixed, swapped)));
        i = end;
    }
}

lh_void
lh_pixel_span_cover_argb(lh_u32_t *at, const lh_byte_t *cover, lh_usize_t count, lh_u32_t src,
                         lh_u32_t alpha)
{
    const lh_u32_t rgb = lh_bit_and(src, 0x00FFFFFFU);

    for (lh_usize_t i = 0U; i < count; ++i)
    {
        const lh_u32_t a = lh_pixel_channel_div255_round(cover[i] * alpha);

        if (a == LH_BYTE_T_MAX)
        {
            at[i] = lh_bit_or(0xFF000000U, rgb);
        }
        else if (a != 0U)
        {
            at[i] = lh_pixel_argb_over(at[i], lh_bit_or(lh_bit_shl(a, 24), rgb));
        }
    }
}

lh_void
lh_pixel_span_cover_rgb565(lh_u16_t *at, const lh_byte_t *cover, lh_usize_t count, lh_u32_t src,
                           lh_u32_t alpha, lh_bool_t swapped)
{
    for (lh_usize_t i = 0U; i < count; ++i)
    {
        const lh_u32_t a = lh_pixel_channel_div255_round(cover[i] * alpha);

        if (a != 0U)
        {
            at[i] = lh_cast_static(
                lh_u16_t,
                lh_pixel_rgb565_order(
                    lh_pixel_rgb565_mix(lh_pixel_rgb565_order(at[i], swapped), src, a), swapped));
        }
    }
}

lh_void
lh_pixel_span_blend_each_argb(lh_u32_t *at, const lh_u32_t *src, lh_usize_t count)
{
    for (lh_usize_t i = 0U; i < count; ++i)
    {
        const lh_u32_t a = lh_pixel_argb_get_channel(src[i], 24);

        if (a == LH_BYTE_T_MAX)
        {
            at[i] = src[i];
        }
        else if (a != 0U)
        {
            at[i] = lh_pixel_argb_over(at[i], src[i]);
        }
    }
}

lh_void
lh_pixel_span_blend_each_rgb565(lh_u16_t *at, const lh_u32_t *src, lh_usize_t count,
                                lh_bool_t swapped)
{
    for (lh_usize_t i = 0U; i < count; ++i)
    {
        const lh_u32_t a = lh_pixel_argb_get_channel(src[i], 24);

        if (a != 0U)
        {
            at[i] = lh_cast_static(
                lh_u16_t, lh_pixel_rgb565_order(
                              lh_pixel_rgb565_mix(lh_pixel_rgb565_order(at[i], swapped), src[i], a),
                              swapped));
        }
    }
}

lh_void
lh_pixel_span_cover_each_argb(lh_u32_t *at, const lh_byte_t *cover, const lh_u32_t *src,
                              lh_usize_t count)
{
    for (lh_usize_t i = 0U; i < count; ++i)
    {
        const lh_u32_t a =
            lh_pixel_channel_div255_round(cover[i] * lh_pixel_argb_get_channel(src[i], 24));
        const lh_u32_t rgb = lh_bit_and(src[i], 0x00FFFFFFU);

        if (a == LH_BYTE_T_MAX)
        {
            at[i] = lh_bit_or(0xFF000000U, rgb);
        }
        else if (a != 0U)
        {
            at[i] = lh_pixel_argb_over(at[i], lh_bit_or(lh_bit_shl(a, 24), rgb));
        }
    }
}

lh_void
lh_pixel_span_cover_each_rgb565(lh_u16_t *at, const lh_byte_t *cover, const lh_u32_t *src,
                                lh_usize_t count, lh_bool_t swapped)
{
    for (lh_usize_t i = 0U; i < count; ++i)
    {
        const lh_u32_t a =
            lh_pixel_channel_div255_round(cover[i] * lh_pixel_argb_get_channel(src[i], 24));

        if (a != 0U)
        {
            at[i] = lh_cast_static(
                lh_u16_t, lh_pixel_rgb565_order(
                              lh_pixel_rgb565_mix(lh_pixel_rgb565_order(at[i], swapped), src[i], a),
                              swapped));
        }
    }
}
