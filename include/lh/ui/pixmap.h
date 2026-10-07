/**
 * @file pixmap.h
 * @brief A buffer of ARGB8888 pixels: ::lh_ui_pixmap_t.
 *
 * `width` x `height` pixels, each one ::lh_u32_t `0xAARRGGBB` with straight
 * alpha (::lh_ui_color_get_argb), rows `stride` pixels apart. The pixels are
 * not owned: a Win32 DIB section, a frame buffer, a test array. Everything
 * here writes inside the pixmap only; the caller cuts spans first.
 *
 * Writes come in two kinds: store (the pixel becomes the color) and blend
 * (::lh_ui_color_over the pixel). A span is one row: the unit a faster
 * blend (SIMD) would replace.
 */

#ifndef LH_UI_PIXMAP_H
#define LH_UI_PIXMAP_H

#include <lh/bool.h>
#include <lh/byte.h>
#include <lh/compiler/extern/c.h>
#include <lh/numeric/fixed/types.h>
#include <lh/ui/color.h>
#include <lh/ui/pixmap/fields.h>
#include <lh/ui/rect.h>
#include <lh/void.h>

/**
 * @struct lh_ui_pixmap
 * @typedef lh_ui_pixmap_t
 * @brief A view of ARGB8888 pixels.
 */
struct lh_ui_pixmap
{
    lh_ui_pixmap_fields(lh_u32_t, lh_s32_t);
};
typedef struct lh_ui_pixmap lh_ui_pixmap_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Fill @p self over @p pixels: @p height rows @p stride pixels apart,
 *        @p width pixels used in each. The pixels are not copied.
 *
 * @p stride is at least @p width.
 */
lh_void
lh_ui_pixmap_init(lh_ui_pixmap_t *self, lh_u32_t *pixels, lh_s32_t width, lh_s32_t height, lh_s32_t stride);

/**
 * @brief No pixels, size zero.
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
 * @brief `(0, 0, width, height)`: the rect every write stays in.
 */
lh_ui_rect_t
lh_ui_pixmap_get_bounds(const lh_ui_pixmap_t *self);

/**
 * @brief First pixel of row @p y.
 */
lh_u32_t *
lh_ui_pixmap_get_row(const lh_ui_pixmap_t *self, lh_s32_t y);

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
 * @brief Blend @p color over pixel (@p x, @p y) (::lh_ui_color_over).
 */
lh_void
lh_ui_pixmap_blend_pixel(lh_ui_pixmap_t *self, lh_s32_t x, lh_s32_t y, const lh_ui_color_t *color);

/**
 * @brief Blend @p color at @p coverage (`0..255`) over pixel (@p x, @p y): an
 *        anti-aliased edge pixel. Nothing at coverage `0`.
 */
lh_void
lh_ui_pixmap_cover_pixel(lh_ui_pixmap_t *self, lh_s32_t x, lh_s32_t y, const lh_ui_color_t *color,
                         lh_byte_t coverage);

/**
 * @brief Store the pixel word @p argb in pixels `x0 .. x1 - 1` of row @p y.
 */
lh_void
lh_ui_pixmap_store_span(lh_ui_pixmap_t *self, lh_s32_t x0, lh_s32_t x1, lh_s32_t y, lh_u32_t argb);

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
 * @brief ::lh_ui_pixmap_fill_span on rows `y0 .. y1 - 1`.
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
