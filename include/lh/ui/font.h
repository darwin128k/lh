/**
 * @file font.h
 * @brief A bitmap font kept in memory: ::lh_ui_font_t.
 *
 * The caller owns the bytes. Glyphs are a dense run of equal cells from
 * ::lh_ui_font_get_first, one advance byte each. A row is packed high bit
 * first, at 1, 2, 4, or 8 bits per pixel, and padded to a whole byte.
 * Codes outside 0..255 are absent. The label does not paint these glyphs.
 */

#ifndef LH_UI_FONT_H
#define LH_UI_FONT_H

#include <lh/bool.h>
#include <lh/byte.h>
#include <lh/compiler/extern/c.h>
#include <lh/math/coord.h>
#include <lh/memory/view.h>
#include <lh/numeric/types.h>
#include <lh/size.h>
#include <lh/ui/font/fields.h>
#include <lh/void.h>

/**
 * @struct lh_ui_font
 * @typedef lh_ui_font_t
 * @brief A view of glyph bitmaps and their advances.
 */
struct lh_ui_font
{
    lh_ui_font_fields(lh_memory_view_t, lh_math_coord_t, lh_byte_t, lh_usize_t);
};
typedef struct lh_ui_font lh_ui_font_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Roboto Regular at 16 pixels, codes 32..126, 4 bits per pixel.
 *
 * Baked from the TTF. The file is not opened at runtime. This is
 * ::LH_LIBRARY_OPTION_UI_FONT unless the build names another font.
 */
extern const lh_ui_font_t lh_ui_font_roboto;

/**
 * @brief Fill @p self over @p glyphs and @p advances.
 *
 * @p count is the number of codes from @p first. @p glyphs holds
 * @p count cells of @p height rows of @p row_bytes. The bytes are not copied.
 */
lh_void
lh_ui_font_init(lh_ui_font_t *self, const lh_byte_t *glyphs, lh_usize_t glyph_size, const lh_byte_t *advances,
                lh_usize_t count, lh_math_coord_t cell_width, lh_math_coord_t height,
                lh_math_coord_t row_bytes, lh_byte_t bpp, lh_byte_t first);

/**
 * @brief Glyph bitmaps of @p self.
 */
lh_memory_view_t
lh_ui_font_get_glyphs(const lh_ui_font_t *self);

/**
 * @brief One advance byte per glyph of @p self.
 */
lh_memory_view_t
lh_ui_font_get_advances(const lh_ui_font_t *self);

/**
 * @brief Width of one cell, in pixels.
 */
lh_math_coord_t
lh_ui_font_get_cell_width(const lh_ui_font_t *self);

/**
 * @brief Height of one cell, in pixels. This is the line height.
 */
lh_math_coord_t
lh_ui_font_get_height(const lh_ui_font_t *self);

/**
 * @brief Bytes in one row of one cell.
 */
lh_math_coord_t
lh_ui_font_get_row_bytes(const lh_ui_font_t *self);

/**
 * @brief Bits per pixel: 1, 2, 4, or 8.
 */
lh_byte_t
lh_ui_font_get_bpp(const lh_ui_font_t *self);

/**
 * @brief First code the run stores.
 */
lh_byte_t
lh_ui_font_get_first(const lh_ui_font_t *self);

/**
 * @brief How many codes the run stores.
 */
lh_usize_t
lh_ui_font_get_count(const lh_ui_font_t *self);

/**
 * @brief Last code @p self stores.
 */
lh_uint_t
lh_ui_font_get_last(const lh_ui_font_t *self);

/**
 * @brief True when @p self stores a glyph for @p code.
 */
lh_bool_t
lh_ui_font_has_code(const lh_ui_font_t *self, lh_uint_t code);

/**
 * @brief Index of @p code in the run, or 0 when @p self has no such glyph.
 */
lh_usize_t
lh_ui_font_get_index(const lh_ui_font_t *self, lh_uint_t code);

/**
 * @brief Bytes in one glyph cell.
 */
lh_usize_t
lh_ui_font_get_cell_bytes(const lh_ui_font_t *self);

/**
 * @brief True when (@p x, @p y) lies inside one cell.
 */
lh_bool_t
lh_ui_font_has_pixel(const lh_ui_font_t *self, lh_math_coord_t x, lh_math_coord_t y);

/**
 * @brief Bytes of the glyph for @p code.
 *
 * The view is empty when @p self has no such glyph.
 */
lh_memory_view_t
lh_ui_font_get_glyph(const lh_ui_font_t *self, lh_uint_t code);

/**
 * @brief Advance of @p code, or 0 when @p self has no such glyph.
 */
lh_byte_t
lh_ui_font_get_advance(const lh_ui_font_t *self, lh_uint_t code);

/**
 * @brief Coverage of pixel (@p x, @p y) in the cell of @p code, in 0..255.
 *
 * A missing glyph and a pixel outside the cell are 0.
 */
lh_byte_t
lh_ui_font_get_coverage(const lh_ui_font_t *self, lh_uint_t code, lh_math_coord_t x, lh_math_coord_t y);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_FONT_H */
