/**
 * @file radii.h
 * @brief A radius for each corner of a box: ::lh_ui_radii_t.
 *
 * ::lh_ui_style_set_radius_v rounds every corner alike, and that is most boxes. A card
 * whose top is rounded and whose bottom sits flush on the one below it -- a tab, a
 * sheet, the head of a panel -- is not, and CSS spells it `border-radius: 8px 8px 0 0`.
 *
 * Coverage is the one-radius coverage (::lh_ui_radius_coverage) with the radius of the
 * corner a pixel is nearest: a pixel in the top-left quarter of the box can only be
 * cut by the top-left corner, as long as no radius is more than half a side -- which
 * ::lh_ui_radii_clamp makes sure of.
 */

#ifndef LH_UI_RADII_H
#define LH_UI_RADII_H

#include <lh/ui/radii/fields.h>
#include <lh/ui/rect.h>

/**
 * @struct lh_ui_radii
 * @brief The radius of each corner.
 */
struct lh_ui_radii
{
    /** Members: ::lh_ui_radii_fields. */
    lh_ui_radii_fields(lh_ui_scalar_t);
};
/** @copybrief lh_ui_radii */
typedef struct lh_ui_radii lh_ui_radii_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief @p self with each corner's radius, clockwise from the top left.
 */
lh_void
lh_ui_radii_init(lh_ui_radii_t *self, lh_ui_scalar_t top_left, lh_ui_scalar_t top_right,
                 lh_ui_scalar_t bottom_right, lh_ui_scalar_t bottom_left);

/**
 * @brief @p self with every corner @p radius.
 */
lh_void
lh_ui_radii_init_all(lh_ui_radii_t *self, lh_ui_scalar_t radius);

/**
 * @brief True when every corner of @p self has the same radius.
 */
lh_bool_t
lh_ui_radii_is_uniform(const lh_ui_radii_t *self);

/**
 * @brief The smallest of the four radii.
 */
lh_ui_scalar_t
lh_ui_radii_get_min(const lh_ui_radii_t *self);

/** @p radius times @p num / @p den when @p num < @p den, else @p radius: the CSS factor,
   kept in the scalar type so a build with no FPU rounds down rather than up. */
lh_ui_scalar_t
lh_ui_radii_scale(lh_ui_scalar_t radius, lh_ui_scalar_t num, lh_ui_scalar_t den);

/**
 * @brief @p self made to fit @p rect, as CSS fits them: when two corners on one side
 *        add up to more than the side, every radius is scaled down by the same
 *        factor, and none is more than half the shorter side.
 */
lh_void
lh_ui_radii_clamp(const lh_ui_radii_t *self, const lh_ui_rect_t *rect, lh_ui_radii_t *out);

/**
 * @brief The radius of the corner of @p rect the pixel (@p x, @p y) is nearest:
 *        the one whose quarter of the box it is in.
 */
lh_ui_scalar_t
lh_ui_radii_get_at(const lh_ui_radii_t *self, const lh_ui_rect_t *rect, lh_s32_t x, lh_s32_t y);

/**
 * @brief The top-left corner radius (level 1).
 *
 * @param self The radii; checked by ::lh_assert_runtime_ref.
 */
lh_ui_scalar_t
lh_ui_radii_get_top_left(const lh_ui_radii_t *self);

/**
 * @brief Store the top-left corner radius (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The radii; checked by ::lh_assert_runtime_ref.
 * @param top_left The new value.
 */
lh_void
lh_ui_radii_set_top_left(lh_ui_radii_t *self, lh_ui_scalar_t top_left);

/**
 * @brief The top-right corner radius (level 1).
 *
 * @param self The radii; checked by ::lh_assert_runtime_ref.
 */
lh_ui_scalar_t
lh_ui_radii_get_top_right(const lh_ui_radii_t *self);

/**
 * @brief Store the top-right corner radius (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The radii; checked by ::lh_assert_runtime_ref.
 * @param top_right The new value.
 */
lh_void
lh_ui_radii_set_top_right(lh_ui_radii_t *self, lh_ui_scalar_t top_right);

/**
 * @brief The bottom-right corner radius (level 1).
 *
 * @param self The radii; checked by ::lh_assert_runtime_ref.
 */
lh_ui_scalar_t
lh_ui_radii_get_bottom_right(const lh_ui_radii_t *self);

/**
 * @brief Store the bottom-right corner radius (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The radii; checked by ::lh_assert_runtime_ref.
 * @param bottom_right The new value.
 */
lh_void
lh_ui_radii_set_bottom_right(lh_ui_radii_t *self, lh_ui_scalar_t bottom_right);

/**
 * @brief The bottom-left corner radius (level 1).
 *
 * @param self The radii; checked by ::lh_assert_runtime_ref.
 */
lh_ui_scalar_t
lh_ui_radii_get_bottom_left(const lh_ui_radii_t *self);

/**
 * @brief Store the bottom-left corner radius (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The radii; checked by ::lh_assert_runtime_ref.
 * @param bottom_left The new value.
 */
lh_void
lh_ui_radii_set_bottom_left(lh_ui_radii_t *self, lh_ui_scalar_t bottom_left);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_RADII_H */
