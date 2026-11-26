/**
 * @file radii.h
 * @brief A radius for each corner of a box: ::lh_gl_radii_t.
 *
 * ::lh_gl_style_set_radius_v rounds every corner alike, and that is most boxes. A card
 * whose top is rounded and whose bottom sits flush on the one below it -- a tab, a
 * sheet, the head of a panel -- is not, and CSS spells it `border-radius: 8px 8px 0 0`.
 *
 * Coverage is the one-radius coverage (::lh_gl_radius_coverage) with the radius of the
 * corner a pixel is nearest: a pixel in the top-left quarter of the box can only be
 * cut by the top-left corner, as long as no radius is more than half a side -- which
 * ::lh_gl_radii_clamp makes sure of.
 */

#ifndef LH_GL_RADII_H
#define LH_GL_RADII_H

#include <lh/gl/radii/fields.h>
#include <lh/gl/rect.h>

/**
 * @struct lh_gl_radii
 * @brief The radius of each corner.
 */
struct lh_gl_radii
{
    /** Members: ::lh_gl_radii_fields. */
    lh_gl_radii_fields(lh_gl_scalar_t);
};
/** @copybrief lh_gl_radii */
typedef struct lh_gl_radii lh_gl_radii_t;

LH_COMPILER_EXTERN_C_BEGIN

/** @p radius times @p num / @p den when @p num < @p den, else @p radius: the CSS factor,
   kept in the scalar type so a build with no FPU rounds down rather than up. */
lh_gl_scalar_t
lh_gl_radii_scale(lh_gl_scalar_t radius, lh_gl_scalar_t num, lh_gl_scalar_t den);

/**
 * @brief Tighten the factor @p num / @p den by one side: when the two corners on it
 *        add up to @p sum, more than the @p side they share, and @p side / @p sum
 *        is smaller than the factor so far, the factor becomes @p side / @p sum.
 *
 * Compared without dividing, so a build with no FPU loses nothing. A side that is
 * not over-full leaves the factor as it was.
 *
 * @param side How long the side is.
 * @param sum  What its two corners add up to.
 * @param num  The factor's numerator, read and written; not checked.
 * @param den  The factor's denominator (> 0), read and written; not checked.
 */
lh_void
lh_gl_radii_tighten(lh_gl_scalar_t side, lh_gl_scalar_t sum, lh_gl_scalar_t *num,
                    lh_gl_scalar_t *den);

/**
 * @brief The top-left corner radius (level 1).
 *
 * @param self The radii; checked by ::lh_assert_runtime_ref.
 */
lh_gl_scalar_t
lh_gl_radii_get_top_left(const lh_gl_radii_t *self);

/**
 * @brief Store the top-left corner radius (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The radii; checked by ::lh_assert_runtime_ref.
 * @param top_left The new value.
 */
lh_void
lh_gl_radii_set_top_left(lh_gl_radii_t *self, lh_gl_scalar_t top_left);

/**
 * @brief The top-right corner radius (level 1).
 *
 * @param self The radii; checked by ::lh_assert_runtime_ref.
 */
lh_gl_scalar_t
lh_gl_radii_get_top_right(const lh_gl_radii_t *self);

/**
 * @brief Store the top-right corner radius (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The radii; checked by ::lh_assert_runtime_ref.
 * @param top_right The new value.
 */
lh_void
lh_gl_radii_set_top_right(lh_gl_radii_t *self, lh_gl_scalar_t top_right);

/**
 * @brief The bottom-right corner radius (level 1).
 *
 * @param self The radii; checked by ::lh_assert_runtime_ref.
 */
lh_gl_scalar_t
lh_gl_radii_get_bottom_right(const lh_gl_radii_t *self);

/**
 * @brief Store the bottom-right corner radius (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The radii; checked by ::lh_assert_runtime_ref.
 * @param bottom_right The new value.
 */
lh_void
lh_gl_radii_set_bottom_right(lh_gl_radii_t *self, lh_gl_scalar_t bottom_right);

/**
 * @brief The bottom-left corner radius (level 1).
 *
 * @param self The radii; checked by ::lh_assert_runtime_ref.
 */
lh_gl_scalar_t
lh_gl_radii_get_bottom_left(const lh_gl_radii_t *self);

/**
 * @brief Store the bottom-left corner radius (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The radii; checked by ::lh_assert_runtime_ref.
 * @param bottom_left The new value.
 */
lh_void
lh_gl_radii_set_bottom_left(lh_gl_radii_t *self, lh_gl_scalar_t bottom_left);

/**
 * @brief @p self with each corner's radius, clockwise from the top left.
 */
lh_void
lh_gl_radii_init(lh_gl_radii_t *self, lh_gl_scalar_t top_left, lh_gl_scalar_t top_right,
                 lh_gl_scalar_t bottom_right, lh_gl_scalar_t bottom_left);

/**
 * @brief True when every corner of @p self has the same radius.
 */
lh_bool_t
lh_gl_radii_is_uniform(const lh_gl_radii_t *self);

/**
 * @brief The smallest of the four radii.
 */
lh_gl_scalar_t
lh_gl_radii_get_min(const lh_gl_radii_t *self);

/**
 * @brief The largest of the four radii.
 */
lh_gl_scalar_t
lh_gl_radii_get_max(const lh_gl_radii_t *self);

/**
 * @brief The one factor that makes every side of a box of @p size hold its two
 *        corners of @p self, as CSS computes it: @p num / @p den, `1 / 1` when every
 *        side already does (::lh_gl_radii_tighten over top, bottom, left, right).
 *
 * @param self The radii, none below `0`.
 * @param size The box.
 * @param num  Receives the numerator; not checked.
 * @param den  Receives the denominator; not checked.
 */
lh_void
lh_gl_radii_get_fit(const lh_gl_radii_t *self, const lh_gl_size_t *size, lh_gl_scalar_t *num,
                    lh_gl_scalar_t *den);

/**
 * @brief The radius of the corner of @p rect the pixel (@p x, @p y) is nearest:
 *        the one whose quarter of the box it is in.
 */
lh_gl_scalar_t
lh_gl_radii_get_at(const lh_gl_radii_t *self, const lh_gl_rect_t *rect, lh_s32_t x, lh_s32_t y);

/**
 * @brief @p self with every corner @p radius.
 */
lh_void
lh_gl_radii_init_all(lh_gl_radii_t *self, lh_gl_scalar_t radius);

/**
 * @brief Every corner of @p self less @p by, none below `0`, into @p out: the
 *        corners of the box a border @p by wide leaves inside.
 *
 * @param self The radii.
 * @param by   How far inside.
 * @param out  Receives the radii; checked by ::lh_assert_runtime_ref.
 */
lh_void
lh_gl_radii_inset(const lh_gl_radii_t *self, lh_gl_scalar_t by, lh_gl_radii_t *out);

/**
 * @brief Every corner of @p self times @p num / @p den (::lh_gl_radii_scale), into
 *        @p out; @p out may be @p self.
 *
 * @param self The radii.
 * @param num  The factor's numerator.
 * @param den  The factor's denominator.
 * @param out  Receives the radii.
 */
lh_void
lh_gl_radii_scale_all(const lh_gl_radii_t *self, lh_gl_scalar_t num, lh_gl_scalar_t den,
                      lh_gl_radii_t *out);

/**
 * @brief Every corner of @p self, none more than @p max, into @p out; @p out may be
 *        @p self.
 *
 * @param self The radii.
 * @param max  The largest a corner may be.
 * @param out  Receives the radii.
 */
lh_void
lh_gl_radii_limit(const lh_gl_radii_t *self, lh_gl_scalar_t max, lh_gl_radii_t *out);

/**
 * @brief @p self made to fit @p rect, as CSS fits them: when two corners on one side
 *        add up to more than the side, every radius is scaled down by the same
 *        factor, and none is more than half the shorter side.
 *
 * Composed: none below `0` (::lh_gl_radii_inset by `0`), the factor
 * (::lh_gl_radii_get_fit), every corner scaled (::lh_gl_radii_scale_all), then none
 * past ::lh_gl_radius_get_max (::lh_gl_radii_limit).
 *
 * @param self The radii.
 * @param rect The box they round.
 * @param out  Receives the radii; may be @p self.
 */
lh_void
lh_gl_radii_clamp(const lh_gl_radii_t *self, const lh_gl_rect_t *rect, lh_gl_radii_t *out);

LH_COMPILER_EXTERN_C_END

#endif /* LH_GL_RADII_H */
