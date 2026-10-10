/**
 * @file font.h
 * @brief A bitmap font kept in memory: ::lh_ui_font_t.
 *
 * Only data and small queries. Glyphs are a set of ::lh_ui_mask_t each cropped to
 * its own ink, with one advance byte and one top (mask top relative to the
 * baseline; up is negative) each, and ::lh_ui_font_range_t says which code point is
 * which glyph. A glyph with no ink is a zero-size mask with a real advance, because
 * the pen still moves over it. A code no range covers has no glyph and advance `0`.
 * The bytes and tables are not owned; `scripts/font.py` bakes them from a TTF.
 * Measuring and drawing text is `lh/ui/text.h`.
 */

#ifndef LH_UI_FONT_H
#define LH_UI_FONT_H

#include <lh/bool.h>
#include <lh/byte.h>
#include <lh/compiler/extern/c.h>
#include <lh/numeric/fixed/types.h>
#include <lh/ui/font/fields.h>
#include <lh/ui/font/range.h>
#include <lh/ui/mask.h>
#include <lh/void.h>

/**
 * @struct lh_ui_font
 * @typedef lh_ui_font_t
 * @brief Glyph masks, advances and tops over bytes the caller owns.
 */
struct lh_ui_font
{
    lh_ui_font_fields(lh_byte_t, lh_ui_mask_t, lh_s32_t, lh_u32_t, lh_ui_font_range_t);
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
 * @brief Fill @p self over @p glyphs, @p advances and @p tops: one entry per
 *        glyph, placed by @p ranges, line @p line_height with @p ascent and a
 *        cap height of @p cap_height. The tables are not copied. A glyph without
 *        ink is a zero-size mask.
 *
 * @p ranges must be ordered and must not overlap: a code in two runs would have
 * two answers, and the lookup walks them in order and takes the first, so a font
 * built wrong draws one of the two glyphs and reports the other in a metric.
 * That is refused here rather than resolved, because a run that is not a run is a
 * table built by hand with no way to tell which half was meant.
 */
lh_void
lh_ui_font_init(lh_ui_font_t *self, const lh_ui_mask_t *glyphs, const lh_byte_t *advances,
                const lh_s32_t *tops, const lh_ui_font_range_t *ranges, lh_s32_t line_height,
                lh_s32_t ascent, lh_s32_t cap_height, lh_u32_t range_count);

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
 * @brief Cap height of @p self: the baseline up to a capital letter, in pixels.
 *
 * The one metric that is about no particular glyph and all of them at once —
 * where a capital letter of @p font starts above the line it sits on. It is
 * what `LH_UI_TRIM_CAPITAL_BASELINE` measures, and it is why a text is centred
 * by it and not by its ink: the ink of a word is as tall as the tallest and the
 * lowest letter in it, so centring it moves the text when the words change,
 * while the cap line is the same for every word a font draws.
 */
lh_s32_t
lh_ui_font_get_cap_height(const lh_ui_font_t *self);

/**
 * @brief How many runs of code points @p self covers.
 *
 * One for a font of Latin alone; three or four for one that also has Cyrillic,
 * because a hole in the coverage (a soft hyphen Roboto has no glyph for) ends a
 * run and starts another.
 */
lh_u32_t
lh_ui_font_get_range_count(const lh_ui_font_t *self);

/**
 * @brief Run @p index of @p self. @p index must be below
 *        ::lh_ui_font_get_range_count.
 */
const lh_ui_font_range_t *
lh_ui_font_get_range(const lh_ui_font_t *self, lh_u32_t index);

/**
 * @brief Lowest code @p self has a glyph for.
 *
 * With Cyrillic beside Latin this is the space, not a statement that the font
 * covers everything up to it — that is what the runs are for.
 */
lh_u32_t
lh_ui_font_get_first(const lh_ui_font_t *self);

/**
 * @brief How many glyphs @p self has in total, over every run.
 */
lh_u32_t
lh_ui_font_get_glyph_count(const lh_ui_font_t *self);

/**
 * @brief True when @p self has a glyph for @p code.
 */
lh_bool_t
lh_ui_font_has_code(const lh_ui_font_t *self, lh_u32_t code);

/**
 * @brief Position of @p code in the glyph tables. @p code must be in a run.
 *
 * The index is into `glyphs` / `advances` / `tops`, so it is **not** the code
 * minus anything: it is the run's base plus how far into the run the code sits.
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
 *         A code in a run with no ink still returns true: @p mask is then
 *         zero-size.
 */
lh_bool_t
lh_ui_font_get_glyph(const lh_ui_font_t *self, lh_u32_t code, lh_ui_mask_t *mask);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_FONT_H */