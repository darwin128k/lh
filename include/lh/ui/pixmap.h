/**
 * @file pixmap.h
 * @brief A buffer of pixels: ::lh_ui_pixmap_t.
 *
 * `width` x `height` pixels in one ::lh_ui_pixmap_format_t (ARGB8888,
 * RGB565 or byte-swapped RGB565), rows `stride` bytes apart. The bytes are not owned: a Win32 DIB
 * section, a microcontroller frame buffer, a test array. Rows and pixels are
 * aligned to the pixel size. Everything here writes inside the pixmap only;
 * the caller cuts spans first.
 *
 * A pixel is read and written as a format word (::lh_ui_pixmap_pack /
 * ::lh_ui_pixmap_unpack). Writes come in two kinds: store (the pixel becomes
 * the color) and blend (::lh_ui_color_over the pixel). Stored runs go
 * through one typed store loop per row (::lh_ui_pixmap_store_words32 /
 * `16`), not a call per pixel.
 */

#ifndef LH_UI_PIXMAP_H
#define LH_UI_PIXMAP_H

#include <lh/bool.h>
#include <lh/byte.h>
#include <lh/byte/ptr.h>
#include <lh/compiler/extern/c.h>
#include <lh/numeric/fixed/types.h>
#include <lh/size.h>
#include <lh/ui/color.h>
#include <lh/ui/pixmap/fields.h>
#include <lh/ui/pixmap/format.h>
#include <lh/ui/rect.h>
#include <lh/void.h>

/**
 * @def LH_UI_PIXMAP_RUN
 * @brief Pixels one row kernel call takes at most (the size of the alpha
 *        buffers callers fill on the stack).
 */
#define LH_UI_PIXMAP_RUN 64

/**
 * @struct lh_ui_pixmap
 * @brief A view of pixels in one format.
 */
struct lh_ui_pixmap
{
    /** Members: ::lh_ui_pixmap_fields. */
    lh_ui_pixmap_fields(lh_byte_t, lh_s32_t, lh_ui_pixmap_format_t);
};
/** @copybrief lh_ui_pixmap */
typedef struct lh_ui_pixmap lh_ui_pixmap_t;

LH_COMPILER_EXTERN_C_BEGIN

/* в”Ђв”Ђ Lifetime and shape в”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђ */

/**
 * @brief True for the 16-bit formats (RGB565, plain or swapped).
 */
lh_bool_t
lh_ui_pixmap_format_is_16(lh_ui_pixmap_format_t format);

/**
 * @brief Bytes one pixel of @p format takes: 4 or 2.
 */
lh_s32_t
lh_ui_pixmap_format_get_bytes(lh_ui_pixmap_format_t format);

/**
 * @brief Fill @p self over @p bits: @p height rows @p stride bytes apart,
 *        @p width pixels of @p format in each. The bytes are not copied.
 *
 * @p stride is at least `width * bytes` and a multiple of the pixel size.
 */
lh_void
lh_ui_pixmap_init(lh_ui_pixmap_t *self, lh_byte_ptr bits, lh_s32_t width, lh_s32_t height, lh_s32_t stride,
                  lh_ui_pixmap_format_t format);

/**
 * @brief No pixels, size zero (ARGB8888).
 */
lh_void
lh_ui_pixmap_init_empty(lh_ui_pixmap_t *self);

/**
 * @brief Width of @p self, in pixels.
 */
lh_s32_t
lh_ui_pixmap_get_width(const lh_ui_pixmap_t *self);

/**
 * @brief Height of @p self, in pixels.
 */
lh_s32_t
lh_ui_pixmap_get_height(const lh_ui_pixmap_t *self);

/**
 * @brief Pixel format of @p self.
 */
lh_ui_pixmap_format_t
lh_ui_pixmap_get_format(const lh_ui_pixmap_t *self);

/**
 * @brief Bytes one pixel of @p self takes.
 */
lh_s32_t
lh_ui_pixmap_get_pixel_bytes(const lh_ui_pixmap_t *self);

/**
 * @brief `(0, 0, width, height)`: the rect every write stays in.
 */
lh_ui_rect_t
lh_ui_pixmap_get_bounds(const lh_ui_pixmap_t *self);

/**
 * @brief First byte of row @p y.
 */
lh_byte_ptr
lh_ui_pixmap_get_row(const lh_ui_pixmap_t *self, lh_s32_t y);

/**
 * @brief First byte of pixel (@p x, @p y).
 */
lh_byte_ptr
lh_ui_pixmap_get_address(const lh_ui_pixmap_t *self, lh_s32_t x, lh_s32_t y);

/* в”Ђв”Ђ Pixels в”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђ */

/**
 * @brief @p word with its two bytes swapped when @p self is
 *        ::lh_ui_pixmap_format_rgb565_swapped, else as is (its own inverse).
 */
lh_u32_t
lh_ui_pixmap_order_16(const lh_ui_pixmap_t *self, lh_u32_t word);

/**
 * @brief @p color as a word of the format of @p self (::lh_ui_color_get_argb
 *        or ::lh_ui_color_get_rgb565).
 */
lh_u32_t
lh_ui_pixmap_pack(const lh_ui_pixmap_t *self, const lh_ui_color_t *color);

/**
 * @brief A word of the format of @p self as a color; RGB565 reads opaque.
 */
lh_ui_color_t
lh_ui_pixmap_unpack(const lh_ui_pixmap_t *self, lh_u32_t word);

/**
 * @brief The stored word of pixel (@p x, @p y).
 */
lh_u32_t
lh_ui_pixmap_read_word(const lh_ui_pixmap_t *self, lh_s32_t x, lh_s32_t y);

/**
 * @brief Store the word @p word in pixel (@p x, @p y) (the low 16 bits for
 *        RGB565).
 */
lh_void
lh_ui_pixmap_write_word(lh_ui_pixmap_t *self, lh_s32_t x, lh_s32_t y, lh_u32_t word);

/**
 * @brief Pixel (@p x, @p y) as a color.
 */
lh_ui_color_t
lh_ui_pixmap_get_pixel(const lh_ui_pixmap_t *self, lh_s32_t x, lh_s32_t y);

/**
 * @brief Store @p color at (@p x, @p y), no blending.
 */
lh_void
lh_ui_pixmap_set_pixel(lh_ui_pixmap_t *self, lh_s32_t x, lh_s32_t y, const lh_ui_color_t *color);

/**
 * @brief Blend @p color over pixel (@p x, @p y) (::lh_ui_color_over); an
 *        opaque @p color is stored without reading the pixel (same result).
 */
lh_void
lh_ui_pixmap_blend_pixel(lh_ui_pixmap_t *self, lh_s32_t x, lh_s32_t y, const lh_ui_color_t *color);

/**
 * @brief Blend @p color at @p coverage (`0..255`) over pixel (@p x, @p y): an
 *        anti-aliased edge pixel. Nothing at `0`; @p color as is at `255`.
 */
lh_void
lh_ui_pixmap_cover_pixel(lh_ui_pixmap_t *self, lh_s32_t x, lh_s32_t y, const lh_ui_color_t *color,
                         lh_byte_t coverage);

/* в”Ђв”Ђ Spans and boxes в”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђ */

/**
 * @brief Store @p word in @p count 32-bit words from @p at, eight per turn.
 */
lh_void
lh_ui_pixmap_store_words32(lh_u32_t *at, lh_usize_t count, lh_u32_t word);

/**
 * @brief Store @p word in @p count 16-bit words from @p at, eight per turn.
 */
lh_void
lh_ui_pixmap_store_words16(lh_u16_t *at, lh_usize_t count, lh_u16_t word);

/**
 * @brief Store @p word in pixels `x0 .. x1 - 1` of row @p y (the low 16 bits
 *        for RGB565): one tight store loop of the pixel width.
 */
lh_void
lh_ui_pixmap_store_span(lh_ui_pixmap_t *self, lh_s32_t x0, lh_s32_t x1, lh_s32_t y, lh_u32_t word);

/* в”Ђв”Ђ Row kernels в”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђ */

/**
 * @brief `0xRRGGBB` of @p src at alpha @p a over opaque @p dst (both
 *        `0x..RRGGBB`): ::lh_ui_color_over_opaque_channel per channel.
 */
lh_u32_t
lh_ui_pixmap_mix_rgb(lh_u32_t dst, lh_u32_t src, lh_u32_t a);

/**
 * @brief The stored word @p dst of @p self with @p src (`0xRRGGBB`) at alpha
 *        @p a over it, through colors: ::lh_ui_color_over for any format and
 *        any destination alpha (the slow, general case).
 */
lh_u32_t
lh_ui_pixmap_mix_any(const lh_ui_pixmap_t *self, lh_u32_t dst, lh_u32_t src, lh_u32_t a);

/**
 * @brief ARGB8888 word @p dst with @p src at alpha @p a over it:
 *        ::lh_ui_color_argb_over on the words, opaque or translucent @p dst.
 */
lh_u32_t
lh_ui_pixmap_mix_argb(const lh_ui_pixmap_t *self, lh_u32_t dst, lh_u32_t src, lh_u32_t a);

/**
 * @brief 16-bit word @p dst (format of @p self) with @p src at alpha @p a
 *        over it; RGB565 is always opaque.
 */
lh_u32_t
lh_ui_pixmap_mix_16(const lh_ui_pixmap_t *self, lh_u32_t dst, lh_u32_t src, lh_u32_t a);

/**
 * @brief Red and blue of an RGB565 @p word (numeric order), each widened to 8
 *        bits in its byte lane: red in bits 16..23, blue in 0..7 (`0x00FF00FF`).
 */
lh_u32_t
lh_ui_pixmap_rb_of_565(lh_u32_t word);

/**
 * @brief Green of an RGB565 @p word (numeric order), widened to 8 bits.
 */
lh_u32_t
lh_ui_pixmap_g_of_565(lh_u32_t word);

/**
 * @brief The RGB565 word (numeric order) of 8-bit red and blue lanes @p rb
 *        (::lh_ui_pixmap_rb_of_565's layout) and 8-bit green @p g, truncated.
 */
lh_u32_t
lh_ui_pixmap_565_of(lh_u32_t rb, lh_u32_t g);

/**
 * @brief One 16-bit pixel @p dst under @p src at alpha @p a: unchanged at `0`,
 *        @p solid (the packed @p src) at `255`, ::lh_ui_pixmap_mix_16 between.
 */
lh_u32_t
lh_ui_pixmap_blend_16_one(const lh_ui_pixmap_t *self, lh_u32_t dst, lh_u32_t src, lh_u32_t solid, lh_u32_t a);

/**
 * @brief Two 16-bit pixels in one 32-bit word @p pair (the low half first in
 *        memory), each through ::lh_ui_pixmap_blend_16_one at its own alpha.
 */
lh_u32_t
lh_ui_pixmap_blend_16_pair(const lh_ui_pixmap_t *self, lh_u32_t pair, lh_u32_t src, lh_u32_t solid, lh_u32_t a_lo,
                           lh_u32_t a_hi);

/**
 * @brief The two pixels at @p at, each through ::lh_ui_pixmap_blend_16_one at its
 *        own alpha with its own 16-bit access (left alone when both alphas are 0).
 */
lh_void
lh_ui_pixmap_blend_16_pair_at(const lh_ui_pixmap_t *self, lh_u16_t *at, const lh_byte_ptr alpha, lh_u32_t src,
                              lh_u32_t solid);

/**
 * @brief Pixel word @p dst (format of @p self) with RGB @p src at alpha @p a
 *        over it: ::lh_ui_pixmap_mix_16 or ::lh_ui_pixmap_mix_argb, by format.
 */
lh_u32_t
lh_ui_pixmap_mix_word(const lh_ui_pixmap_t *self, lh_u32_t dst, lh_u32_t src, lh_u32_t a);

/**
 * @brief The word pixel @p dst (format of @p self) becomes with @p color painted
 *        over it (::lh_ui_color_over): @p color packed when it is opaque, @p dst
 *        when it is transparent, ::lh_ui_pixmap_mix_word otherwise.
 *
 * The one per-pixel "over" of this module: ::lh_ui_pixmap_blend_pixel and
 * ::lh_ui_pixmap_fill_colors_span are this, read and written back, and no
 * ::lh_ui_color_t is built for the destination.
 */
lh_u32_t
lh_ui_pixmap_over_word(const lh_ui_pixmap_t *self, lh_u32_t dst, const lh_ui_color_t *color);

/**
 * @brief ::lh_ui_pixmap_over_word for an ARGB8888 @p self: the format already
 *        known, so a loop over a row pays for no format test per pixel.
 */
lh_u32_t
lh_ui_pixmap_over_argb(const lh_ui_pixmap_t *self, lh_u32_t dst, const lh_ui_color_t *color);

/**
 * @brief ::lh_ui_pixmap_over_word for a 16-bit @p self (RGB565, either byte
 *        order): the format already known.
 */
lh_u32_t
lh_ui_pixmap_over_16(const lh_ui_pixmap_t *self, lh_u32_t dst, const lh_ui_color_t *color);

/**
 * @brief @p src (`0xRRGGBB`) as an RGB565 word, native order.
 */
lh_u32_t
lh_ui_pixmap_pack_rgb(const lh_ui_pixmap_t *self, lh_u32_t src);

/**
 * @brief Blend @p src over @p count ARGB8888 words at @p at, each at its own
 *        @p alpha (`0` skipped, `255` stored).
 */
lh_void
lh_ui_pixmap_blend_alpha_32(const lh_ui_pixmap_t *self, lh_u32_t *at, const lh_byte_ptr alpha, lh_usize_t count,
                            lh_u32_t src);

/**
 * @brief ::lh_ui_pixmap_blend_alpha_32 for the 16-bit formats.
 */
lh_void
lh_ui_pixmap_blend_alpha_16(const lh_ui_pixmap_t *self, lh_u16_t *at, const lh_byte_ptr alpha, lh_usize_t count,
                            lh_u32_t src);

/**
 * @brief SSE2 form of ::lh_ui_pixmap_blend_alpha_32, four pixels at a time.
 *
 * Blocks of four, the last one to three pixels included: they are read into a
 * block and written back word by word. The scalar kernel branches per pixel three times вЂ” on `alpha[i]`, on it being
 * `255`, and (inside ::lh_ui_pixmap_mix_argb) on whether the destination is
 * opaque at all вЂ” and rebuilds a word per pixel. All three are removable: the
 * general form `(src * a + dst * (255 - a) + 127) / 255` already *is* the
 * `a == 0` case (it returns `dst`) and the `a == 255` case (it returns `src`),
 * so one branch-free pass covers every alpha, and `255` as the divisor becomes
 * `(x + 1 + (x >> 8)) >> 8` with no divide at all.
 *
 * A translucent *destination* takes the general formula, with a divisor per
 * pixel: a block of four with one in it goes through
 * ::lh_ui_pixmap_blend_alpha_32_over_sse2, which is exact too, so the result is
 * bit-identical to the scalar kernel for every input; nothing is approximated
 * away.
 *
 * Only meaningful when `LH_LIBRARY_OPTION_SIMD_HAVE_SSE2` is on and the running
 * CPU reports SSE2 (::lh_cpu_simd_has_sse2); ::lh_ui_pixmap_blend_alpha_32 is
 * what to call, and it dispatches. Exposed so the tier can be tested against
 * the scalar kernel directly instead of only through the dispatcher.
 */
lh_void
lh_ui_pixmap_blend_alpha_32_sse2(const lh_ui_pixmap_t *self, lh_u32_t *at, const lh_byte_ptr alpha, lh_usize_t count,
                                 lh_u32_t src);

/**
 * @brief One block of ::lh_ui_pixmap_blend_alpha_32_sse2: exactly four words at
 *        @p at, pixel `i` at `alpha[i]`. A block with a translucent word in it
 *        goes through ::lh_ui_pixmap_blend_alpha_32_over_quad_sse2.
 */
lh_void
lh_ui_pixmap_blend_alpha_32_quad_sse2(lh_u32_t *at, const lh_byte_ptr alpha, lh_u32_t src);

/**
 * @brief One block of ::lh_ui_pixmap_blend_alpha_32_over_sse2: exactly four words.
 */
lh_void
lh_ui_pixmap_blend_alpha_32_over_quad_sse2(lh_u32_t *at, const lh_byte_ptr alpha, lh_u32_t src);

/**
 * @brief Blend @p src (`0xRRGGBB`) over the first @p count (1 .. 8) ARGB8888
 *        words at @p at, pixel `i` at byte `i` of @p cover: eight coverages in a
 *        register, as ::lh_ui_mask_get_coverage_8 reads them. A short count is
 *        read into the register and written back word by word; nothing past it is
 *        touched.
 */
lh_void
lh_ui_pixmap_blend_cover_32_sse2(lh_u32_t *at, lh_usize_t count, lh_u64_t cover, lh_u32_t src);

/**
 * @brief SSE2 ::lh_ui_color_argb_over of @p src at each @p alpha onto @p count
 *        ARGB8888 words at @p at, for any destination alpha, four pixels a step.
 *
 * The general "over": each channel is divided back by the result alpha, which
 * is a different divisor per pixel. The division is done in single precision and
 * is exact: every numerator is an integer below 2^24, so it is representable, and
 * `divps` rounds correctly; a quotient that is not whole sits at least 1/65025 from
 * the next integer, more than half an ulp below 256, so truncating it is the
 * integer division the scalar kernel does. A zero @p alpha leaves the pixel as it
 * was, as in ::lh_ui_pixmap_blend_alpha_32.
 *
 * ::lh_ui_pixmap_blend_alpha_32_sse2 runs this on a block with a translucent
 * pixel in it; exposed so it can be tested and reused on its own.
 */
lh_void
lh_ui_pixmap_blend_alpha_32_over_sse2(lh_u32_t *at, const lh_byte_ptr alpha, lh_usize_t count, lh_u32_t src);

/**
 * @brief SSE2 form of ::lh_ui_pixmap_blend_alpha_16, eight pixels at a time.
 *
 * Same three branches removed as in ::lh_ui_pixmap_blend_alpha_32_sse2, and the
 * same branch-free formula covering every alpha including 0 and 255. RGB565 is
 * the MCU format, so this is the kernel that path actually runs.
 *
 * What makes it short is that the three channels sit at fixed bit positions
 * inside each 16-bit word: red is lifted out of all eight pixels with one shift
 * and one mask, green with another, blue with a third, giving three registers of
 * eight lanes each, one channel per register and one pixel per lane. Widening
 * the stored bits to a byte is the shift pair `(v << up) | (v >> down)` and
 * narrowing is a shift, so nothing ever has to be shuffled between lanes and pixel k of @p alpha is lane k of all
 * three planes. A swapped format (::lh_ui_pixmap_format_rgb565_swapped) costs one
 * 16-bit byte swap on the way in and one on the way out.
 *
 * Only meaningful when `LH_LIBRARY_OPTION_SIMD_HAVE_SSE2` is on and the running
 * CPU reports SSE2 (::lh_cpu_simd_has_sse2); ::lh_ui_pixmap_blend_alpha_16 is
 * what to call, and it dispatches.
 */
lh_void
lh_ui_pixmap_blend_alpha_16_sse2(const lh_ui_pixmap_t *self, lh_u16_t *at, const lh_byte_ptr alpha, lh_usize_t count,
                                 lh_u32_t src);

/**
 * @brief The portable form of ::lh_ui_pixmap_blend_alpha_16: two pixels a 32-bit
 *        word where @p at is word-aligned (::lh_ui_pixmap_blend_16_pair_at), the
 *        unaligned first and odd last pixel alone. What runs without SSE2.
 */
lh_void
lh_ui_pixmap_blend_alpha_16_words(const lh_ui_pixmap_t *self, lh_u16_t *at, const lh_byte_ptr alpha, lh_usize_t count,
                                  lh_u32_t src);

/**
 * @brief One block of ::lh_ui_pixmap_blend_alpha_16_sse2: exactly eight RGB565
 *        words at @p at, pixel `i` at `alpha[i]`; @p swap for
 *        ::lh_ui_pixmap_format_rgb565_swapped.
 */
lh_void
lh_ui_pixmap_blend_alpha_16_octet_sse2(lh_u16_t *at, const lh_byte_ptr alpha, lh_u32_t src, lh_bool_t swap);

/**
 * @brief ::lh_ui_pixmap_blend_cover_32_sse2 for RGB565: the first @p count
 *        (1 .. 8) words at @p at, pixel `i` at byte `i` of @p cover; @p swap for
 *        ::lh_ui_pixmap_format_rgb565_swapped.
 */
lh_void
lh_ui_pixmap_blend_cover_16_sse2(lh_u16_t *at, lh_usize_t count, lh_u64_t cover, lh_u32_t src, lh_bool_t swap);

/**
 * @brief The row kernel: blend @p src (`0xRRGGBB`) over pixels `x0 .. x1 - 1`
 *        of row @p y, pixel `x0 + i` at `alpha[i]`; at most
 *        ::LH_UI_PIXMAP_RUN pixels. The same result as
 *        ::lh_ui_pixmap_blend_pixel per pixel, without its per-pixel calls.
 */
lh_void
lh_ui_pixmap_blend_alpha_span(lh_ui_pixmap_t *self, lh_s32_t x0, lh_s32_t x1, lh_s32_t y, lh_u32_t src,
                              const lh_byte_ptr alpha);

/**
 * @brief Blend @p src (`0xRRGGBB`) over the @p count (1 .. 8) pixels of row @p y
 *        from @p x, pixel `x + i` at byte `i` of @p cover: the coverages a mask
 *        row hands over eight at a time (::lh_ui_mask_get_coverage_8), blended
 *        straight from the register in every format. What
 *        ::lh_ui_pixmap_blend_alpha_span does with the same coverages in a buffer.
 */
lh_void
lh_ui_pixmap_blend_cover_8(lh_ui_pixmap_t *self, lh_s32_t x, lh_s32_t y, lh_s32_t count, lh_u64_t cover, lh_u32_t src);

/**
 * @brief Blend @p color over pixels `x0 .. x1 - 1` of row @p y (the row
 *        kernel, one alpha for all).
 */
lh_void
lh_ui_pixmap_blend_span(lh_ui_pixmap_t *self, lh_s32_t x0, lh_s32_t x1, lh_s32_t y, const lh_ui_color_t *color);

/**
 * @brief Paint @p color over pixels `x0 .. x1 - 1` of row @p y: a store when
 *        it is opaque (the result is the same), else a blend.
 */
lh_void
lh_ui_pixmap_fill_span(lh_ui_pixmap_t *self, lh_s32_t x0, lh_s32_t x1, lh_s32_t y, const lh_ui_color_t *color);

/**
 * @brief Paint pixels `x0 .. x1 - 1` of row @p y each with its own colour,
 *        `colors[0]` the first: ::lh_ui_pixmap_over_word per pixel, so a pixel
 *        comes out as it would from a fill of its colour.
 *
 * A gradient changes colour every pixel or two, so the run is walked pixel by
 * pixel in either format rather than split into one-colour fills.
 */
lh_void
lh_ui_pixmap_fill_colors_span(lh_ui_pixmap_t *self, lh_s32_t x0, lh_s32_t x1, lh_s32_t y,
                              const lh_ui_color_t *colors);

/**
 * @brief ::lh_ui_pixmap_store_span on rows `y0 .. y1 - 1`.
 */
lh_void
lh_ui_pixmap_store_box(lh_ui_pixmap_t *self, lh_s32_t x0, lh_s32_t y0, lh_s32_t x1, lh_s32_t y1, lh_u32_t word);

/**
 * @brief ::lh_ui_pixmap_blend_span on rows `y0 .. y1 - 1`.
 */
lh_void
lh_ui_pixmap_blend_box(lh_ui_pixmap_t *self, lh_s32_t x0, lh_s32_t y0, lh_s32_t x1, lh_s32_t y1,
                       const lh_ui_color_t *color);

/**
 * @brief Paint @p color over the box: ::lh_ui_pixmap_store_box when opaque,
 *        else ::lh_ui_pixmap_blend_box.
 */
lh_void
lh_ui_pixmap_fill_box(lh_ui_pixmap_t *self, lh_s32_t x0, lh_s32_t y0, lh_s32_t x1, lh_s32_t y1,
                      const lh_ui_color_t *color);

/**
 * @brief Store @p color in every pixel of @p self.
 */
lh_void
lh_ui_pixmap_clear(lh_ui_pixmap_t *self, const lh_ui_color_t *color);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_PIXMAP_H */
