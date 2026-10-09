/**
 * @file font.h
 * @brief A bitmap font kept in memory: ::lh_ui_font_t.
 *
 * Only data and small queries. Glyphs are a dense run of ::lh_ui_mask_t from
 * code `first`, each cropped to its own ink, with one advance byte and one
 * top (mask top relative to the baseline; up is negative) each. A glyph with
 * no ink is a zero-size mask. Codes outside the run have no glyph and
 * advance `0`. The bytes and tables are not owned; `scripts/font.py` bakes
 * them from a TTF. Measuring and drawing text is `lh/ui/text.h`.
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
 * @brief Glyph masks, advances and tops over bytes the caller owns.
 */
struct lh_ui_font
{
    lh_ui_font_fields(lh_byte_t, lh_ui_mask_t, lh_s32_t, lh_u32_t);
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
 * @brief Fill @p self over @p glyphs, @p advances and @p tops: @p count
 *        glyphs from code @p first, line @p line_height with @p ascent.
 *        The tables are not copied. A glyph without ink is a zero-size mask.
 */
lh_void
lh_ui_font_init(lh_ui_font_t *self, const lh_ui_mask_t *glyphs, const lh_byte_t *advances,
                const lh_s32_t *tops, lh_s32_t line_height, lh_s32_t ascent, lh_byte_t first,
                lh_u32_t count);

/**
 * @brief Ascent of @p self: baseline from the top of the line, in pixels.
 */
lh_s32_t
lh_ui_font_get_ascent(const lh_ui_font_t *self);

/**
 * @brief Height of one line, in pixels (`ascent + descent`).
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
 * @brief How far the pen moves after @p code; `0` when there is no glyph.
 */
lh_s32_t
lh_ui_font_get_advance(const lh_ui_font_t *self, lh_u32_t code);

/**
 * @brief Top of the mask of @p code relative to the baseline (up is negative);
 *        `0` when there is no glyph.
 */
lh_s32_t
lh_ui_font_get_top(const lh_ui_font_t *self, lh_u32_t code);

/**
 * @brief Where the ink of @p code starts, measured from the top of the line
 *        box: the baseline (::lh_ui_font_get_ascent) plus
 *        ::lh_ui_font_get_top.
 *
 * The line box is not the ink. A font whose line is 22 px and ascent 17 puts a
 * capital letter's ink well below the middle of that box, so a line centred by
 * its box draws its text low — this is the number that says where the pixels
 * actually are. A code with no ink gives a rect of no height.
 */
lh_s32_t
lh_ui_font_get_ink_top(const lh_ui_font_t *self, lh_u32_t code);

/**
 * @brief Where the ink of @p code ends, measured from the top of the line box:
 *        ::lh_ui_font_get_ink_top plus the height of its mask.
 */
lh_s32_t
lh_ui_font_get_ink_bottom(const lh_ui_font_t *self, lh_u32_t code);

/**
 * @brief Fill @p mask with the glyph of @p code.
 *
 * @return ::lh_bool_false (and @p mask untouched) when there is no glyph.
 *         A code in the run with no ink still returns true: @p mask is then
 *         zero-size.
 */
lh_bool_t
lh_ui_font_get_glyph(const lh_ui_font_t *self, lh_u32_t code, lh_ui_mask_t *mask);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_FONT_H */
