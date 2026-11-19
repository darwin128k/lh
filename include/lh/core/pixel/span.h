/**
 * @file span.h
 * @brief A run of pixels in one row: laid over with one colour, or through a mask.
 *
 * The inner loops of a software renderer, on plain words: `lh_u32_t` for
 * `0xAARRGGBB`, `lh_u16_t` for RGB565. Nothing here knows a pixmap, a clip or a
 * colour type; the caller has clipped the run and found its first pixel, and
 * the span only walks it.
 *
 * Two kinds of run, each with one colour or a colour per pixel (`_each`, a
 * gradient's row):
 *
 * - **blend** — one colour at one alpha over the whole run. An opaque colour is a
 *   store (::lh_memory_set_u32, ::lh_memory_set_u16); a translucent one is mixed
 *   once per **run of equal destination words**, not once per pixel, because a
 *   card over a flat background meets the same word again and again;
 * - **cover** — one colour through a row of a mask (a glyph, an anti-aliased
 *   edge): every pixel has its own coverage, which scales the colour's alpha.
 *
 * Both are what gl's pixmap ran before (`lh_gl_pixmap_blend_const_32` and its
 * neighbours); the code lives here now so that gl and gx share one copy.
 */

#ifndef LH_PIXEL_SPAN_H
#define LH_PIXEL_SPAN_H

#include <lh/core/bool.h>
#include <lh/core/compiler/extern/c.h>
#include <lh/core/size.h>
#include <lh/core/void.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Lay @p src at @p alpha over @p count `0xAARRGGBB` words at @p at.
 *
 * Alpha 0 does nothing; 255 stores `0xFF` | @p src; anything between mixes once
 * per run of equal destination words (::lh_pixel_argb_over).
 *
 * @param at    The first word; @p count words must be writable.
 * @param count How many.
 * @param src   The colour, `0x..RRGGBB` (its alpha byte is ignored).
 * @param alpha Its alpha, 0..255.
 */
lh_void
lh_pixel_span_blend_argb(lh_u32_t *at, lh_usize_t count, lh_u32_t src, lh_u32_t alpha);

/**
 * @brief Lay @p src at @p alpha over @p count RGB565 words at @p at.
 *
 * Alpha 0 does nothing; 255 stores the 565 word of @p src; anything between
 * mixes once per run of equal destination words (::lh_pixel_rgb565_mix).
 *
 * @param at      The first word; @p count words must be writable.
 * @param count   How many.
 * @param src     The colour, `0x..RRGGBB`.
 * @param alpha   Its alpha, 0..255.
 * @param swapped True when the memory holds each word's bytes swapped.
 */
lh_void
lh_pixel_span_blend_rgb565(lh_u16_t *at, lh_usize_t count, lh_u32_t src, lh_u32_t alpha,
                           lh_bool_t swapped);

/**
 * @brief Lay @p src at @p alpha over @p count `0xAARRGGBB` words at @p at,
 *        each scaled by its own coverage from @p cover.
 *
 * @param at    The first word; @p count words must be writable.
 * @param cover @p count coverages, 0..255.
 * @param count How many.
 * @param src   The colour, `0x..RRGGBB`.
 * @param alpha Its alpha, 0..255.
 */
lh_void
lh_pixel_span_cover_argb(lh_u32_t *at, const lh_byte_t *cover, lh_usize_t count, lh_u32_t src,
                         lh_u32_t alpha);

/**
 * @brief Lay @p src at @p alpha over @p count RGB565 words at @p at, each scaled
 *        by its own coverage from @p cover.
 *
 * @param at      The first word; @p count words must be writable.
 * @param cover   @p count coverages, 0..255.
 * @param count   How many.
 * @param src     The colour, `0x..RRGGBB`.
 * @param alpha   Its alpha, 0..255.
 * @param swapped True when the memory holds each word's bytes swapped.
 */
lh_void
lh_pixel_span_cover_rgb565(lh_u16_t *at, const lh_byte_t *cover, lh_usize_t count, lh_u32_t src,
                           lh_u32_t alpha, lh_bool_t swapped);

/**
 * @brief Lay @p count colours of @p src over as many `0xAARRGGBB` words at @p at,
 *        each at its own alpha: a gradient's row.
 *
 * An opaque colour is stored, a clear one leaves the word, anything between is
 * mixed (::lh_pixel_argb_over).
 *
 * @param at    The first word; @p count words must be writable.
 * @param src   @p count colours, `0xAARRGGBB`; may not overlap @p at.
 * @param count How many.
 */
lh_void
lh_pixel_span_blend_each_argb(lh_u32_t *at, const lh_u32_t *src, lh_usize_t count);

/**
 * @brief Lay @p count colours of @p src over as many RGB565 words at @p at, each at
 *        its own alpha (::lh_pixel_rgb565_mix).
 *
 * @param at      The first word; @p count words must be writable.
 * @param src     @p count colours, `0xAARRGGBB`.
 * @param count   How many.
 * @param swapped True when the memory holds each word's bytes swapped.
 */
lh_void
lh_pixel_span_blend_each_rgb565(lh_u16_t *at, const lh_u32_t *src, lh_usize_t count,
                                lh_bool_t swapped);

/**
 * @brief Lay @p count colours of @p src over as many `0xAARRGGBB` words at @p at,
 *        each at its own alpha scaled by its own coverage from @p cover: a
 *        gradient through a mask.
 *
 * @param at    The first word; @p count words must be writable.
 * @param cover @p count coverages, 0..255.
 * @param src   @p count colours, `0xAARRGGBB`.
 * @param count How many.
 */
lh_void
lh_pixel_span_cover_each_argb(lh_u32_t *at, const lh_byte_t *cover, const lh_u32_t *src,
                              lh_usize_t count);

/**
 * @brief Lay @p count colours of @p src over as many RGB565 words at @p at, each at
 *        its own alpha scaled by its own coverage from @p cover.
 *
 * @param at      The first word; @p count words must be writable.
 * @param cover   @p count coverages, 0..255.
 * @param src     @p count colours, `0xAARRGGBB`.
 * @param count   How many.
 * @param swapped True when the memory holds each word's bytes swapped.
 */
lh_void
lh_pixel_span_cover_each_rgb565(lh_u16_t *at, const lh_byte_t *cover, const lh_u32_t *src,
                                lh_usize_t count, lh_bool_t swapped);

LH_COMPILER_EXTERN_C_END

#endif /* LH_PIXEL_SPAN_H */
