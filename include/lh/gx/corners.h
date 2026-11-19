/**
 * @file corners.h
 * @brief One length per corner of a rectangle: ::lh_gx_corners_t.
 *
 * Four lengths, clockwise from the top left — the order of CSS's
 * `border-radius`. A style keeps the radius of each corner of a box in one
 * (::lh_gx_style_get_radius), so a tab rounded on top only, a bubble with one
 * sharp corner or a pill are the same box with other corners.
 *
 * It is a value and nothing more: the caller owns it, it points at nothing, and
 * copying it copies the four lengths. What a length means — and how far it is
 * clamped — is the reader's: the box (`lh/gx/box.h`) takes each as a radius,
 * at most half the shorter side.
 */

#ifndef LH_GX_CORNERS_H
#define LH_GX_CORNERS_H

#include <lh/gx/corners/fields.h>
#include <lh/gx/scalar.h>

/**
 * @struct lh_gx_corners
 * @brief A length per corner. Members: ::lh_gx_corners_fields.
 */
struct lh_gx_corners
{
    /** Members: ::lh_gx_corners_fields. */
    lh_gx_corners_fields(lh_gx_scalar_t);
};
/** @copybrief lh_gx_corners */
typedef struct lh_gx_corners lh_gx_corners_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The top left corner's length (level 1).
 *
 * @param self The corners; checked by ::lh_assert_runtime_ref.
 */
lh_gx_scalar_t
lh_gx_corners_get_top_left(const lh_gx_corners_t *self);

/**
 * @brief Store the top left corner's length (level 1).
 *
 * @param self     The corners; checked by ::lh_assert_runtime_ref.
 * @param top_left The length.
 */
lh_void
lh_gx_corners_set_top_left(lh_gx_corners_t *self, lh_gx_scalar_t top_left);

/**
 * @brief The top right corner's length (level 1).
 *
 * @param self The corners; checked by ::lh_assert_runtime_ref.
 */
lh_gx_scalar_t
lh_gx_corners_get_top_right(const lh_gx_corners_t *self);

/**
 * @brief Store the top right corner's length (level 1).
 *
 * @param self      The corners; checked by ::lh_assert_runtime_ref.
 * @param top_right The length.
 */
lh_void
lh_gx_corners_set_top_right(lh_gx_corners_t *self, lh_gx_scalar_t top_right);

/**
 * @brief The bottom right corner's length (level 1).
 *
 * @param self The corners; checked by ::lh_assert_runtime_ref.
 */
lh_gx_scalar_t
lh_gx_corners_get_bottom_right(const lh_gx_corners_t *self);

/**
 * @brief Store the bottom right corner's length (level 1).
 *
 * @param self         The corners; checked by ::lh_assert_runtime_ref.
 * @param bottom_right The length.
 */
lh_void
lh_gx_corners_set_bottom_right(lh_gx_corners_t *self, lh_gx_scalar_t bottom_right);

/**
 * @brief The bottom left corner's length (level 1).
 *
 * @param self The corners; checked by ::lh_assert_runtime_ref.
 */
lh_gx_scalar_t
lh_gx_corners_get_bottom_left(const lh_gx_corners_t *self);

/**
 * @brief Store the bottom left corner's length (level 1).
 *
 * @param self        The corners; checked by ::lh_assert_runtime_ref.
 * @param bottom_left The length.
 */
lh_void
lh_gx_corners_set_bottom_left(lh_gx_corners_t *self, lh_gx_scalar_t bottom_left);

/**
 * @brief Set all four lengths, clockwise from the top left.
 *
 * @param self         The corners; checked by the setters.
 * @param top_left     The top left corner's.
 * @param top_right    The top right corner's.
 * @param bottom_right The bottom right corner's.
 * @param bottom_left  The bottom left corner's.
 */
lh_void
lh_gx_corners_init(lh_gx_corners_t *self, lh_gx_scalar_t top_left, lh_gx_scalar_t top_right,
                   lh_gx_scalar_t bottom_right, lh_gx_scalar_t bottom_left);

/**
 * @brief The same @p length on every corner.
 *
 * @param self   The corners; checked by the setters.
 * @param length The length.
 */
lh_void
lh_gx_corners_init_all(lh_gx_corners_t *self, lh_gx_scalar_t length);

LH_COMPILER_EXTERN_C_END

#endif /* LH_GX_CORNERS_H */
