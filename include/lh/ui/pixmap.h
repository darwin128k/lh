/**
 * @file pixmap.h
 * @brief A buffer of pixels: ::lh_ui_pixmap_t.
 *
 * `width` x `height` pixels in one ::lh_ui_pixmap_format_t (ARGB8888 or
 * RGB565), rows `stride` bytes apart. The bytes are not owned: a Win32 DIB
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
struct lh_ui_pixmap
{
    lh_ui_pixmap_fields(lh_byte_t, lh_s32_t, lh_ui_pixmap_format_t);
};
typedef struct lh_ui_pixmap lh_ui_pixmap_t;

LH_COMPILER_EXTERN_C_BEGIN

/* ── Lifetime and shape ──────────────────────────────────────────────────── */

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

/**
 * @brief Blend @p color over pixels `x0 .. x1 - 1` of row @p y.
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
