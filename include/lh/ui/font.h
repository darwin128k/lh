/**
 * @file font.h
 * @brief A bitmap font kept in memory: ::lh_ui_font_t.
 *
 * Only data and small queries. Glyphs are a dense run of equal cells from
 * code `first`, each an ::lh_ui_mask_t already placed on the baseline, with
 * one advance byte each: the pen moves by the advance, a line is `height`
 * tall. Codes outside the run have no glyph and advance `0`. The bytes are
 * not owned; `scripts/font.py` bakes them from a TTF. Measuring and drawing
 * text is `lh/ui/text.h`.
 */

#ifndef LH_UI_FONT_H
#define LH_UI_FONT_H

#include <lh/bool.h>
#include <lh/byte.h>
#include <lh/compiler/extern/c.h>
#include <lh/numeric/fixed/types.h>
#include <lh/ui/font/fields.h>
#include <lh/ui/mask.h>
#include <lh/void.h>

/**
 * @struct lh_ui_font
 * @typedef lh_ui_font_t
 * @brief Glyph cells and advances over bytes the caller owns.
 */
struct lh_ui_font
{
    lh_ui_font_fields(lh_byte_t, lh_s32_t, lh_u32_t);
};
typedef struct lh_ui_font lh_ui_font_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The font a new style starts with: the built-in Roboto
 *        (`lh/ui/font/roboto.h`) under ::LH_LIBRARY_OPTION_UI_FONT_ROBOTO,
 *        else ::lh_null.
 */
const lh_ui_font_t *
lh_ui_font_get_default(lh_void);

/**
 * @brief Fill @p self over @p bits and @p advances: @p count glyphs from code
 *        @p first, cells of @p cell_width x @p height at @p bpp, rows of
 *        @p row_bytes. The bytes are not copied.
 */
lh_void
lh_ui_font_init(lh_ui_font_t *self, const lh_byte_t *bits, const lh_byte_t *advances, lh_s32_t cell_width,
                lh_s32_t height, lh_s32_t row_bytes, lh_byte_t bpp, lh_byte_t first, lh_u32_t count);

/**
 * @brief Width of one cell, in pixels (the widest glyph).
 */
lh_s32_t
lh_ui_font_get_cell_width(const lh_ui_font_t *self);

/**
 * @brief Height of one cell, in pixels: the line height.
 */
lh_s32_t
lh_ui_font_get_line_height(const lh_ui_font_t *self);

/**
 * @brief First code with a glyph.
 */
lh_u32_t
lh_ui_font_get_first(const lh_ui_font_t *self);

/**
 * @brief Number of glyphs.
 */
lh_u32_t
lh_ui_font_get_count(const lh_ui_font_t *self);

/**
 * @brief True when @p self has a glyph for @p code.
 */
lh_bool_t
lh_ui_font_has_code(const lh_ui_font_t *self, lh_u32_t code);

/**
 * @brief Position of @p code in the run. @p code must be in it.
 */
lh_u32_t
lh_ui_font_get_index(const lh_ui_font_t *self, lh_u32_t code);

/**
 * @brief Bytes of one cell: `height * row_bytes`.
 */
lh_u32_t
lh_ui_font_get_cell_bytes(const lh_ui_font_t *self);

/**
 * @brief How far the pen moves after @p code; `0` when there is no glyph.
 */
lh_s32_t
lh_ui_font_get_advance(const lh_ui_font_t *self, lh_u32_t code);

/**
 * @brief Fill @p mask with the cell of @p code.
 *
 * @return ::lh_bool_false (and @p mask untouched) when there is no glyph.
 */
lh_bool_t
lh_ui_font_get_glyph(const lh_ui_font_t *self, lh_u32_t code, lh_ui_mask_t *mask);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_FONT_H */
