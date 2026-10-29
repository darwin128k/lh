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

#include <lh/ui/font/fields.h>
#include <lh/ui/font/range.h>
#include <lh/ui/mask.h>

/**
 * @struct lh_ui_font
 * @brief Glyph masks, advances and tops over bytes the caller owns.
 */
struct lh_ui_font
{
    /** Members: ::lh_ui_font_fields. */
    lh_ui_font_fields(lh_byte_t, lh_ui_mask_t, lh_s32_t, lh_u32_t, lh_ui_font_range_t);
};
/** @copybrief lh_ui_font */
typedef struct lh_ui_font lh_ui_font_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The font a new style starts with: the built-in Roboto
 *        (`lh/ui/font/roboto.h`) under ::LH_LIBRARY_OPTION_UI_FONT_ROBOTO,
 *        else ::lh_null.
 *
 * @return The built-in font, or ::lh_null when it is not built in.
 */
const lh_ui_font_t *lh_ui_font_get_default(lh_void);

/**
 * @brief Whether the runs are in order and none of them overlaps the one before.
 *
 * The lookup walks them in order and takes the first run that holds the code, so an
 * overlap is not a wrong answer but **two**: the glyph drawn is one and the advance
 * measured can be the other, and which of the two a test sees depends on which table
 * the test read. That is the same shape as two functions that agree with each other
 * and disagree with the protocol, and it is caught here where the table is given over
 * rather than on a screen.
 *
 * @param ranges The runs to check, in order.
 * @param count  How many runs @p ranges holds.
 * @return ::lh_bool_true when every run is non-empty and none overlaps the one
 *         before it.
 */
lh_bool_t
lh_ui_font_runs_are_a_runs(const lh_ui_font_range_t *ranges, lh_u32_t count);

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
 *
 * @param self        Font to fill.
 * @param glyphs      Glyph masks, one per glyph. Not copied.
 * @param advances    Advance of each glyph, one per glyph. Not copied.
 * @param tops        Top of each glyph's ink, one per glyph. Not copied.
 * @param ranges      The runs the codes sit in, in order. Not copied.
 * @param line_height Height of one line, in pixels.
 * @param ascent      Baseline down from the top of the line, in pixels.
 * @param cap_height  Baseline up to a capital letter, in pixels.
 * @param range_count How many runs @p ranges holds.
 */
lh_void
lh_ui_font_init(lh_ui_font_t *self, const lh_ui_mask_t *glyphs, const lh_byte_ptr advances,
                const lh_s32_t *tops, const lh_ui_font_range_t *ranges, lh_s32_t line_height,
                lh_s32_t ascent, lh_s32_t cap_height, lh_u32_t range_count);

/**
 * @brief Ascent of @p self: baseline from the top of the line, in pixels.
 *
 * @param self Font to read.
 * @return The ascent, in pixels.
 */
lh_s32_t
lh_ui_font_get_ascent(const lh_ui_font_t *self);

/**
 * @brief Height of one line, in pixels (`ascent + descent`).
 *
 * @param self Font to read.
 * @return The height of one line, in pixels.
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
 *
 * @param self Font to read.
 * @return The cap height, in pixels.
 */
lh_s32_t
lh_ui_font_get_cap_height(const lh_ui_font_t *self);

/**
 * @brief How many runs of code points @p self covers.
 *
 * One for a font of Latin alone; three or four for one that also has Cyrillic,
 * because a hole in the coverage (a soft hyphen Roboto has no glyph for) ends a
 * run and starts another.
 *
 * @param self Font to read.
 * @return The number of runs.
 */
lh_u32_t
lh_ui_font_get_range_count(const lh_ui_font_t *self);

/**
 * @brief Run @p index of @p self. @p index must be below
 *        ::lh_ui_font_get_range_count.
 *
 * @param self  Font to read.
 * @param index Run to read.
 * @return The run at @p index.
 */
const lh_ui_font_range_t *
lh_ui_font_get_range(const lh_ui_font_t *self, lh_u32_t index);

/**
 * @brief Lowest code @p self has a glyph for.
 *
 * With Cyrillic beside Latin this is the space, not a statement that the font
 * covers everything up to it — that is what the runs are for.
 *
 * @param self Font to read.
 * @return The lowest code @p self has a glyph for; `0` when it has no runs.
 */
lh_u32_t
lh_ui_font_get_first(const lh_ui_font_t *self);

/**
 * @brief How many glyphs @p self has in total, over every run.
 *
 * @param self Font to read.
 * @return The number of glyphs.
 */
lh_u32_t
lh_ui_font_get_glyph_count(const lh_ui_font_t *self);

/**
 * @brief True when @p self has a glyph for @p code.
 *
 * @param self Font to read.
 * @param code Code to look for.
 * @return ::lh_bool_true when @p self has a glyph for @p code.
 */
lh_bool_t
lh_ui_font_has_code(const lh_ui_font_t *self, lh_u32_t code);

/**
 * @brief Position of @p code in the glyph tables. @p code must be in a run.
 *
 * The index is into `glyphs` / `advances` / `tops`, so it is **not** the code
 * minus anything: it is the run's base plus how far into the run the code sits.
 *
 * @param self Font to read.
 * @param code Code to look up.
 * @return The index of the glyph of @p code.
 */
lh_u32_t
lh_ui_font_get_index(const lh_ui_font_t *self, lh_u32_t code);

/**
 * @brief The run of @p self that holds @p code, or ::lh_null when none does.
 *
 * The one lookup every per-code query of this module makes; the runs are sorted,
 * so the walk stops at the first run that starts past @p code.
 *
 * @param self Font to read.
 * @param code Code to look up.
 * @return The run that holds @p code, or ::lh_null when none does.
 */
const lh_ui_font_range_t *
lh_ui_font_get_range_of(const lh_ui_font_t *self, lh_u32_t code);

/**
 * @brief Find the glyph index of @p code: one run lookup, then every metric of the
 *        glyph is an array read (::lh_ui_font_get_advance_at, ...).
 *
 * What a loop over a line of text wants: ::lh_ui_font_get_advance,
 * ::lh_ui_font_get_top and ::lh_ui_font_get_glyph each look the code up again.
 *
 * @param self  Font to read.
 * @param code  Code to look up.
 * @param index Receives the index when the code is in @p self.
 * @return ::lh_bool_false, @p index untouched, when @p self has no glyph for @p code.
 */
lh_bool_t
lh_ui_font_find_index(const lh_ui_font_t *self, lh_u32_t code, lh_u32_t *index);

/**
 * @brief Advance of the glyph at @p index (from ::lh_ui_font_find_index).
 *
 * @param self  Font to read.
 * @param index Glyph index.
 * @return The advance of the glyph at @p index.
 */
lh_s32_t
lh_ui_font_get_advance_at(const lh_ui_font_t *self, lh_u32_t index);

/**
 * @brief Top of the ink of the glyph at @p index, from the ascent line
 *        (::lh_ui_font_get_top for an index).
 *
 * @param self  Font to read.
 * @param index Glyph index.
 * @return The top of the ink of the glyph at @p index.
 */
lh_s32_t
lh_ui_font_get_top_at(const lh_ui_font_t *self, lh_u32_t index);

/**
 * @brief The mask of the glyph at @p index (::lh_ui_font_get_glyph for an index).
 *
 * @param self  Font to read.
 * @param index Glyph index.
 * @return The mask of the glyph at @p index.
 */
const lh_ui_mask_t *
lh_ui_font_get_glyph_at(const lh_ui_font_t *self, lh_u32_t index);

/**
 * @brief How far the pen moves after @p code; `0` when there is no glyph.
 *
 * @param self Font to read.
 * @param code Code to look up.
 * @return The advance of @p code.
 */
lh_s32_t
lh_ui_font_get_advance(const lh_ui_font_t *self, lh_u32_t code);

/**
 * @brief Top of the mask of @p code relative to the baseline (up is negative);
 *        `0` when there is no glyph.
 *
 * @param self Font to read.
 * @param code Code to look up.
 * @return The top of the mask of @p code.
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
 *
 * @param self Font to read.
 * @param code Code to look up.
 * @return Where the ink of @p code starts, down from the top of the line box.
 */
lh_s32_t
lh_ui_font_get_ink_top(const lh_ui_font_t *self, lh_u32_t code);

/**
 * @brief Where the ink of @p code ends, measured from the top of the line box:
 *        ::lh_ui_font_get_ink_top plus the height of its mask.
 *
 * @param self Font to read.
 * @param code Code to look up.
 * @return Where the ink of @p code ends, down from the top of the line box.
 */
lh_s32_t
lh_ui_font_get_ink_bottom(const lh_ui_font_t *self, lh_u32_t code);

/**
 * @brief Fill @p mask with the glyph of @p code.
 *
 * @param self Font to read.
 * @param code Code to look up.
 * @param mask Receives the glyph of @p code.
 * @return ::lh_bool_false (and @p mask untouched) when there is no glyph.
 *         A code in a run with no ink still returns true: @p mask is then
 *         zero-size.
 */
lh_bool_t
lh_ui_font_get_glyph(const lh_ui_font_t *self, lh_u32_t code, lh_ui_mask_t *mask);

/**
 * @brief The coverage masks of the glyphs (level 1).
 *
 * @param self The font; checked by ::lh_assert_runtime_ref.
 */
const lh_ui_mask_t *
lh_ui_font_get_glyphs(const lh_ui_font_t *self);

/**
 * @brief Store the coverage masks of the glyphs (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The font; checked by ::lh_assert_runtime_ref.
 * @param glyphs The new value.
 */
lh_void
lh_ui_font_set_glyphs(lh_ui_font_t *self, const lh_ui_mask_t * glyphs);

/**
 * @brief How far each glyph moves the pen (level 1).
 *
 * @param self The font; checked by ::lh_assert_runtime_ref.
 */
const lh_byte_t *
lh_ui_font_get_advances(const lh_ui_font_t *self);

/**
 * @brief Store how far each glyph moves the pen (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The font; checked by ::lh_assert_runtime_ref.
 * @param advances The new value.
 */
lh_void
lh_ui_font_set_advances(lh_ui_font_t *self, const lh_byte_t * advances);

/**
 * @brief The top of each glyph above the baseline (level 1).
 *
 * @param self The font; checked by ::lh_assert_runtime_ref.
 */
const lh_s32_t *
lh_ui_font_get_tops(const lh_ui_font_t *self);

/**
 * @brief Store the top of each glyph above the baseline (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The font; checked by ::lh_assert_runtime_ref.
 * @param tops The new value.
 */
lh_void
lh_ui_font_set_tops(lh_ui_font_t *self, const lh_s32_t * tops);

/**
 * @brief The code point ranges the font covers (level 1).
 *
 * @param self The font; checked by ::lh_assert_runtime_ref.
 */
const lh_ui_font_range_t *
lh_ui_font_get_ranges(const lh_ui_font_t *self);

/**
 * @brief Store the code point ranges the font covers (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The font; checked by ::lh_assert_runtime_ref.
 * @param ranges The new value.
 */
lh_void
lh_ui_font_set_ranges(lh_ui_font_t *self, const lh_ui_font_range_t * ranges);

/**
 * @brief Store the height of one line (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The font; checked by ::lh_assert_runtime_ref.
 * @param line_height The new value.
 */
lh_void
lh_ui_font_set_line_height(lh_ui_font_t *self, lh_s32_t line_height);

/**
 * @brief Store how far the font reaches above the baseline (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The font; checked by ::lh_assert_runtime_ref.
 * @param ascent The new value.
 */
lh_void
lh_ui_font_set_ascent(lh_ui_font_t *self, lh_s32_t ascent);

/**
 * @brief Store the height of a capital letter (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The font; checked by ::lh_assert_runtime_ref.
 * @param cap_height The new value.
 */
lh_void
lh_ui_font_set_cap_height(lh_ui_font_t *self, lh_s32_t cap_height);

/**
 * @brief Store how many code point ranges there are (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The font; checked by ::lh_assert_runtime_ref.
 * @param range_count The new value.
 */
lh_void
lh_ui_font_set_range_count(lh_ui_font_t *self, lh_u32_t range_count);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_FONT_H */
