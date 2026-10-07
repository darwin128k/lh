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
#include <lh/compiler/extern/c.h>
#include <lh/numeric/fixed/types.h>
#include <lh/size.h>
#include <lh/ui/color.h>
#include <lh/ui/pixmap/fields.h>
#include <lh/ui/pixmap/format.h>
#include <lh/ui/rect.h>
#include <lh/void.h>

/**
 * @struct lh_ui_pixmap
 * @typedef lh_ui_pixmap_t
 * @brief A view of pixels in one format.
 */
/**
 * @def LH_UI_PIXMAP_RUN
 * @brief Pixels one row kernel call takes at most (the size of the alpha
 *        buffers callers fill on the stack).
 */
#define LH_UI_PIXMAP_RUN 64

struct lh_ui_pixmap
{
    lh_ui_pixmap_fields(lh_byte_t, lh_s32_t, lh_ui_pixmap_format_t);
};
typedef struct lh_ui_pixmap lh_ui_pixmap_t;

LH_COMPILER_EXTERN_C_BEGIN

/* ── Lifetime and shape ──────────────────────────────────────────────────── */

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
lh_ui_pixmap_init(lh_ui_pixmap_t *self, lh_byte_t *bits, lh_s32_t width, lh_s32_t height, lh_s32_t stride,
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
lh_byte_t *
lh_ui_pixmap_get_row(const lh_ui_pixmap_t *self, lh_s32_t y);

/**
 * @brief First byte of pixel (@p x, @p y).
 */
lh_byte_t *
lh_ui_pixmap_get_address(const lh_ui_pixmap_t *self, lh_s32_t x, lh_s32_t y);

/* ── Pixels ──────────────────────────────────────────────────────────────── */

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

/* ── Spans and boxes ─────────────────────────────────────────────────────── */

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

/* ── Row kernels ─────────────────────────────────────────────────────────── */

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
 * @brief ARGB8888 word @p dst with @p src at alpha @p a over it: the integer
 *        path for an opaque @p dst, ::lh_ui_pixmap_mix_any otherwise.
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
 * @brief @p src (`0xRRGGBB`) as an RGB565 word, native order.
 */
lh_u32_t
lh_ui_pixmap_pack_rgb(const lh_ui_pixmap_t *self, lh_u32_t src);

/**
 * @brief Blend @p src over @p count ARGB8888 words at @p at, each at its own
 *        @p alpha (`0` skipped, `255` stored).
 */
lh_void
lh_ui_pixmap_blend_alpha_32(const lh_ui_pixmap_t *self, lh_u32_t *at, const lh_byte_t *alpha, lh_usize_t count,
                            lh_u32_t src);

/**
 * @brief ::lh_ui_pixmap_blend_alpha_32 for the 16-bit formats.
 */
lh_void
lh_ui_pixmap_blend_alpha_16(const lh_ui_pixmap_t *self, lh_u16_t *at, const lh_byte_t *alpha, lh_usize_t count,
                            lh_u32_t src);

/**
 * @brief SSE2 form of ::lh_ui_pixmap_blend_alpha_32, four pixels at a time.
 *
 * The scalar kernel branches per pixel three times — on `alpha[i]`, on it being
 * `255`, and (inside ::lh_ui_pixmap_mix_argb) on whether the destination is
 * opaque at all — and rebuilds a word per pixel. All three are removable: the
 * general form `(src * a + dst * (255 - a) + 127) / 255` already *is* the
 * `a == 0` case (it returns `dst`) and the `a == 255` case (it returns `src`),
 * so one branch-free pass covers every alpha, and `255` as the divisor becomes
 * `(x + 1 + (x >> 8)) >> 8` with no divide at all.
 *
 * The remaining scalar branch is the translucent *destination*, which
 * ::lh_ui_pixmap_mix_argb handles by a different formula. A pixel whose
 * destination alpha is not `255` is detected here and redone scalar, so the
 * result is bit-identical to the scalar kernel for every input; nothing is
 * approximated away.
 *
 * Only meaningful when `LH_LIBRARY_OPTION_SIMD_HAVE_SSE2` is on and the running
 * CPU reports SSE2 (::lh_cpu_simd_has_sse2); ::lh_ui_pixmap_blend_alpha_32 is
 * what to call, and it dispatches. Exposed so the tier can be tested against
 * the scalar kernel directly instead of only through the dispatcher.
 */
lh_void
lh_ui_pixmap_blend_alpha_32_sse2(const lh_ui_pixmap_t *self, lh_u32_t *at, const lh_byte_t *alpha, lh_usize_t count,
                                 lh_u32_t src);

/**
 * @brief The row kernel: blend @p src (`0xRRGGBB`) over pixels `x0 .. x1 - 1`
 *        of row @p y, pixel `x0 + i` at `alpha[i]`; at most
 *        ::LH_UI_PIXMAP_RUN pixels. The same result as
 *        ::lh_ui_pixmap_blend_pixel per pixel, without its per-pixel calls.
 */
lh_void
lh_ui_pixmap_blend_alpha_span(lh_ui_pixmap_t *self, lh_s32_t x0, lh_s32_t x1, lh_s32_t y, lh_u32_t src,
                              const lh_byte_t *alpha);

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
