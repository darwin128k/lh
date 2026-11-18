/**
 * @file glyph.h
 * @brief One character as a font gives it: ::lh_gx_glyph_t.
 *
 * What the `get_glyph` slot of a font (::lh_gx_font_t) answers for a code point:
 * the coverage of the ink (::lh_pixel_mask_t, cropped to it), how far the pen
 * moves after it, and where the mask sits against the baseline. A glyph with no
 * ink — a space — is an empty mask with a real advance. A glyph is a value on the
 * stack of whoever draws the text; its mask points into the font's bytes.
 */

#ifndef LH_GX_GLYPH_H
#define LH_GX_GLYPH_H

#include <lh/core/pixel/mask.h>
#include <lh/gx/glyph/fields.h>

/**
 * @struct lh_gx_glyph
 * @brief The ink of one character, its advance and its top.
 */
struct lh_gx_glyph
{
    /** Members: ::lh_gx_glyph_fields. */
    lh_gx_glyph_fields(lh_pixel_mask_t, lh_s32_t);
};
/** @copybrief lh_gx_glyph */
typedef struct lh_gx_glyph lh_gx_glyph_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The coverage of the ink (level 1).
 *
 * @param self The glyph; checked by ::lh_assert_runtime_ref.
 */
const lh_pixel_mask_t *
lh_gx_glyph_get_mask(const lh_gx_glyph_t *self);

/**
 * @brief Store the coverage of the ink (level 1).
 *
 * @param self The glyph; checked by ::lh_assert_runtime_ref.
 * @param mask The mask; its bytes are the font's.
 */
lh_void
lh_gx_glyph_set_mask(lh_gx_glyph_t *self, lh_pixel_mask_t mask);

/**
 * @brief How far the pen moves after the glyph, in pixels (level 1).
 *
 * @param self The glyph; checked by ::lh_assert_runtime_ref.
 */
lh_s32_t
lh_gx_glyph_get_advance(const lh_gx_glyph_t *self);

/**
 * @brief Store how far the pen moves after the glyph (level 1).
 *
 * @param self    The glyph; checked by ::lh_assert_runtime_ref.
 * @param advance Pixels.
 */
lh_void
lh_gx_glyph_set_advance(lh_gx_glyph_t *self, lh_s32_t advance);

/**
 * @brief Where the top of the mask is, from the baseline; up is negative (level 1).
 *
 * @param self The glyph; checked by ::lh_assert_runtime_ref.
 */
lh_s32_t
lh_gx_glyph_get_top(const lh_gx_glyph_t *self);

/**
 * @brief Store where the top of the mask is, from the baseline (level 1).
 *
 * @param self The glyph; checked by ::lh_assert_runtime_ref.
 * @param top  Pixels; up is negative.
 */
lh_void
lh_gx_glyph_set_top(lh_gx_glyph_t *self, lh_s32_t top);

/**
 * @brief A glyph of @p mask, moving the pen by @p advance, its top @p top from the
 *        baseline.
 *
 * @param self    The glyph; must not be null.
 * @param mask    The coverage of the ink.
 * @param advance How far the pen moves after it.
 * @param top     Where the top of the mask is, from the baseline.
 */
lh_void
lh_gx_glyph_init(lh_gx_glyph_t *self, lh_pixel_mask_t mask, lh_s32_t advance, lh_s32_t top);

LH_COMPILER_EXTERN_C_END

#endif /* LH_GX_GLYPH_H */
