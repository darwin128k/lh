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
 * neighbours); the code lives here now, in the core, and gl calls it.
 */

#ifndef LH_PIXEL_SPAN_H
#define LH_PIXEL_SPAN_H

#include <lh/config.h>
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
 * @brief Lay @p rgb over @p count `0xAARRGGBB` words at @p at, all of which are the
 *        one opaque word @p dst, each at its own alpha from @p alpha.
 *
 * The run's word is taken apart once and every pixel mixed two channels at a time
 * (::lh_pixel_channel_mix_pair): a shadow's edge and a glyph's row lie over one
 * background. The words are ::lh_pixel_argb_over's, `0` leaving the word and `255`
 * storing `0xFF` | @p rgb. Moved from gl (where it was `lh_gl_color_argb_over_opaque_span`), the
 * path a board with no SIMD runs.
 *
 * @param at    The first word; @p count words, each @p dst.
 * @param alpha @p count alphas, 0..255.
 * @param count How many.
 * @param dst   The run's word; its alpha is 255.
 * @param rgb   The colour, `0x..RRGGBB` (its alpha byte is ignored).
 */
lh_void
lh_pixel_span_over_opaque_argb(lh_u32_t *at, const lh_byte_t *alpha, lh_usize_t count, lh_u32_t dst,
                               lh_u32_t rgb);

/**
 * @brief Lay @p rgb over @p count `0xAARRGGBB` words at @p at, each at its own alpha
 *        from @p alpha, a run of one destination word at a time: an opaque run
 *        through ::lh_pixel_span_over_opaque_argb, a translucent one a pixel at a
 *        time (::lh_pixel_argb_over). The scalar kernel.
 *
 * @param at    The first word; @p count words must be writable.
 * @param alpha @p count alphas, 0..255.
 * @param count How many.
 * @param rgb   The colour, `0x..RRGGBB`.
 */
lh_void
lh_pixel_span_blend_vary_argb(lh_u32_t *at, const lh_byte_t *alpha, lh_usize_t count, lh_u32_t rgb);

/**
 * @brief Lay the opaque colour @p rgb over @p count RGB565 words at @p at, pixel
 *        `i` at its own alpha `alpha[i]`, one pixel at a time: a clear pixel is
 *        left, a full one stored, the rest mixed (::lh_pixel_rgb565_mix).
 *
 * Portable: what ::lh_pixel_span_blend_alphas_rgb565 runs without SSE2, and what a
 * board without SIMD draws its text and corners with. Moved from gl
 * (`lh_gl_pixmap_blend_alpha_16_words`), which calls the core now.
 *
 * @param at      The first word; @p count words must be writable.
 * @param alpha   @p count alphas, 0..255.
 * @param count   How many.
 * @param rgb     The colour, `0x..RRGGBB`.
 * @param swapped True when the memory holds each word's bytes swapped.
 */
lh_void
lh_pixel_span_blend_vary_rgb565(lh_u16_t *at, const lh_byte_t *alpha, lh_usize_t count,
                                lh_u32_t rgb, lh_bool_t swapped);

#if LH_LIBRARY_OPTION_SIMD_HAVE_SSE2
/**
 * @brief SSE2 form of ::lh_pixel_span_blend_vary_argb, four pixels at a time, the
 *        last one to three a block too.
 *
 * Over an opaque block every channel is `(src * a + dst * (255 - a) + 127) / 255` in
 * 16-bit lanes, with `255` as the divisor turned into shifts: that form already is
 * `a == 0` (the word) and `a == 255` (the colour), so a block needs no branch per
 * pixel. A block with a translucent word goes through
 * ::lh_pixel_span_over_alphas_sse2_argb. Every word is the scalar kernel's, bit for
 * bit. Only for a CPU that reports SSE2 (::lh_cpu_simd_has_sse2);
 * ::lh_pixel_span_blend_alphas_argb chooses.
 *
 * @param at    The first word; @p count words must be writable.
 * @param alpha @p count alphas, 0..255.
 * @param count How many.
 * @param rgb   The colour, `0x..RRGGBB`.
 */
lh_void
lh_pixel_span_blend_alphas_sse2_argb(lh_u32_t *at, const lh_byte_t *alpha, lh_usize_t count,
                                     lh_u32_t rgb);

/**
 * @brief ::lh_pixel_span_blend_alphas_sse2_argb of the first @p count (1 .. 8) words
 *        at @p at, pixel `i` at byte `i` of @p alphas: eight alphas in a register, as
 *        a mask hands a glyph's row over eight at a time. A short count is read
 *        into the register and written back word by word; nothing past it is
 *        touched. Moved from gl (`lh_gl_pixmap_blend_cover_32_sse2`, which calls it).
 *
 * @param at     The first word; @p count words must be writable.
 * @param count  1 .. 8.
 * @param alphas The alphas, pixel `i` at byte `i` (the low byte first).
 * @param rgb    The colour, `0x..RRGGBB`.
 */
lh_void
lh_pixel_span_blend_alphas8_sse2_argb(lh_u32_t *at, lh_usize_t count, lh_u64_t alphas,
                                      lh_u32_t rgb);

/**
 * @brief SSE2 ::lh_pixel_argb_over of @p rgb at each alpha onto @p count words, for
 *        any destination alpha, four pixels a step.
 *
 * Each channel is divided back by the result alpha, a divisor per pixel, in single
 * precision, and exactly: every numerator is an integer below 2^24, so it is
 * representable, and a quotient that is not whole sits at least 1/65025 from the
 * next integer, more than half an ulp below 256, so truncating it is the integer
 * division the scalar kernel does. A zero alpha leaves the word.
 *
 * @param at    The first word; @p count words must be writable.
 * @param alpha @p count alphas, 0..255.
 * @param count How many.
 * @param rgb   The colour, `0x..RRGGBB`.
 */
lh_void
lh_pixel_span_over_alphas_sse2_argb(lh_u32_t *at, const lh_byte_t *alpha, lh_usize_t count,
                                    lh_u32_t rgb);
/**
 * @brief ::lh_pixel_span_blend_vary_rgb565 eight pixels at a time with SSE2, bit
 *        for bit the same words.
 *
 * Each channel is a plane of eight lanes lifted out of the words with a shift and
 * a mask, widened to eight bits, blended and narrowed back; the last one to seven
 * pixels are a block too. A swapped format costs one byte swap per block each
 * way. Moved from gl (`lh_gl_pixmap_blend_alpha_16_sse2`), which calls the core
 * now.
 *
 * Only defined when `LH_LIBRARY_OPTION_SIMD_HAVE_SSE2` is on; call it only when
 * the CPU reports SSE2 (::lh_cpu_simd_has_sse2).
 *
 * @param at      The first word; @p count words must be writable.
 * @param alpha   @p count alphas, 0..255.
 * @param count   How many.
 * @param rgb     The colour, `0x..RRGGBB`.
 * @param swapped True when the memory holds each word's bytes swapped.
 */
lh_void
lh_pixel_span_blend_alphas_sse2_rgb565(lh_u16_t *at, const lh_byte_t *alpha, lh_usize_t count,
                                       lh_u32_t rgb, lh_bool_t swapped);

/**
 * @brief One block of ::lh_pixel_span_blend_alphas_sse2_rgb565 with its alphas in
 *        a register: @p count (1 .. 8) pixels, pixel `i` at byte `i` of @p alphas
 *        -- how a glyph row read eight pixels a step
 *        (::lh_pixel_mask_get_coverage_8) is laid without a buffer. Moved from gl
 *        (`lh_gl_pixmap_blend_cover_16_sse2`).
 *
 * Only defined when `LH_LIBRARY_OPTION_SIMD_HAVE_SSE2` is on.
 *
 * @param at      The first word; @p count words must be writable.
 * @param count   How many, 1 to 8.
 * @param alphas  Eight alphas, pixel `i` in byte `i`.
 * @param rgb     The colour, `0x..RRGGBB`.
 * @param swapped True when the memory holds each word's bytes swapped.
 */
lh_void
lh_pixel_span_blend_alphas8_sse2_rgb565(lh_u16_t *at, lh_usize_t count, lh_u64_t alphas,
                                        lh_u32_t rgb, lh_bool_t swapped);

#endif

/**
 * @brief Lay @p rgb over @p count `0xAARRGGBB` words at @p at, each at its own alpha
 *        from @p alpha (`0` leaves the word, `255` stores the colour): a shadow's
 *        alphas, a glyph's coverage.
 *
 * The clear pixels at either end are not walked. A row shorter than 16 pixels — a
 * corner's, a glyph's — goes to the SSE2 kernel whole when the CPU has it: looking
 * for runs in it cost more than its blocks. In a longer row a run of one alpha at
 * least four pixels long is one colour laid over (::lh_pixel_span_blend_argb), and
 * the shorter runs between go through the SSE2 kernel
 * (::lh_pixel_span_blend_alphas_sse2_argb), or the scalar one without SSE2
 * (::lh_pixel_span_blend_vary_argb). The words are the same either way. Moved from
 * gl (where it was `lh_gl_pixmap_blend_alpha_32`), which calls it now.
 *
 * Measured on `bench/lvgl/` (i7-3770K, 2026-11-19), against the build before the
 * move, run alternately: gl's shadow frame from 687–694 to 534–540 (the short
 * rows and the clear ends); gl's text frame stayed at 236–242. Every picture is the
 * same to the byte.
 *
 * @param at    The first word; @p count words must be writable.
 * @param alpha @p count alphas, 0..255.
 * @param count How many.
 * @param rgb   The colour, `0x..RRGGBB`.
 */
lh_void
lh_pixel_span_blend_alphas_argb(lh_u32_t *at, const lh_byte_t *alpha, lh_usize_t count,
                                lh_u32_t rgb);

/**
 * @brief Lay the opaque colour @p rgb over @p count RGB565 words at @p at, pixel
 *        `i` at its own alpha: ::lh_pixel_span_blend_alphas_sse2_rgb565 when the
 *        CPU has SSE2, ::lh_pixel_span_blend_vary_rgb565 otherwise -- the same words
 *        either way.
 *
 * @param at      The first word; @p count words must be writable.
 * @param alpha   @p count alphas, 0..255.
 * @param count   How many.
 * @param rgb     The colour, `0x..RRGGBB`.
 * @param swapped True when the memory holds each word's bytes swapped.
 */
lh_void
lh_pixel_span_blend_alphas_rgb565(lh_u16_t *at, const lh_byte_t *alpha, lh_usize_t count,
                                  lh_u32_t rgb, lh_bool_t swapped);

/**
 * @brief Lay @p src at @p alpha over @p count `0xAARRGGBB` words at @p at,
 *        each scaled by its own coverage from @p cover.
 *
 * @param at    The first word; @p count words must be writable.
 * @param cover @p count coverages, 0..255.
 * @param count How many.
 * @param src   The colour, `0x..RRGGBB`.
 * @param alpha Its alpha, 0..255; at 255 the coverages are the alphas, else each is
 *              scaled first, and either way ::lh_pixel_span_blend_alphas_argb lays
 *              them.
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
