/**
 * @file range.h
 * @brief One run of code points a font has glyphs for: ::lh_ui_font_range_t.
 *
 * A font covers code points in runs, not in one line. `scripts/font.py` drops the
 * code points the font has no glyph for -- a soft hyphen, a spacing diaeresis --
 * and a run ends either side of such a hole, which is the whole reason this is a
 * list rather than a `first` and a `count`.
 */

#ifndef LH_UI_FONT_RANGE_H
#define LH_UI_FONT_RANGE_H

#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/numeric/fixed/types.h>
#include <lh/ui/font/range/fields.h>
#include <lh/void.h>

/**
 * @struct lh_ui_font_range
 * @typedef lh_ui_font_range_t
 * @brief @p length consecutive code points from @p first, glyphs at @p base.
 */
struct lh_ui_font_range
{
    lh_ui_font_range_fields(lh_u32_t, lh_u32_t);
};
typedef struct lh_ui_font_range lh_ui_font_range_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Fill @p self: @p length consecutive codes from @p first, whose glyphs
 *        start at @p base in the font's tables.
 */
lh_void
lh_ui_font_range_init(lh_ui_font_range_t *self, lh_u32_t first, lh_u32_t length, lh_u32_t base);

/**
 * @brief First code of @p self.
 */
lh_u32_t
lh_ui_font_range_get_first(const lh_ui_font_range_t *self);

/**
 * @brief How many consecutive codes @p self covers.
 */
lh_u32_t
lh_ui_font_range_get_length(const lh_ui_font_range_t *self);

/**
 * @brief Where @p first's glyph sits in the font's tables.
 */
lh_u32_t
lh_ui_font_range_get_base(const lh_ui_font_range_t *self);

/**
 * @brief Last code of @p self, or the one before @p first when it is empty.
 *
 * An empty run has no last code, and saying so as `first - 1` is what makes
 * ::lh_ui_font_range_has answer false for everything without a special case.
 */
lh_u32_t
lh_ui_font_range_get_last(const lh_ui_font_range_t *self);

/**
 * @brief True when @p code is one of @p self's.
 *
 * The half-open test `code - first < length` rather than two comparisons: a run
 * that has rolled over its `lh_u32_t` (an empty one, or one ending at
 * `0xFFFFFFFF`) must answer false, and the subtraction answers it while
 * `code <= first + length - 1` answers whatever the addition wrapped to.
 */
lh_bool_t
lh_ui_font_range_has(const lh_ui_font_range_t *self, lh_u32_t code);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_FONT_RANGE_H */