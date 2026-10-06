/**
 * @file axis.h
 * @brief One screen axis, ::lh_ui_axis_t, and geometry read and built along it.
 *
 * Code that works the same way across or down (a scrollbar, a list) takes an
 * axis and goes through these helpers instead of branching on x / y itself.
 */

#ifndef LH_UI_AXIS_H
#define LH_UI_AXIS_H

#include <lh/compiler/extern/c.h>
#include <lh/ui/point.h>
#include <lh/ui/rect.h>
#include <lh/ui/scalar.h>
#include <lh/ui/size.h>
#include <lh/void.h>

/**
 * @enum lh_ui_axis
 * @brief Horizontal (`x`, width) or vertical (`y`, height).
 */
typedef enum lh_ui_axis
{
    lh_ui_axis_horizontal = 0, /**< `x` and width; left to right. */
    lh_ui_axis_vertical = 1    /**< `y` and height; top to bottom. */
} lh_ui_axis_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The coordinate of @p self on @p axis.
 */
lh_ui_scalar_t
lh_ui_point_get_along(const lh_ui_point_t *self, lh_ui_axis_t axis);

/**
 * @brief Replace the coordinate of @p self on @p axis with @p value.
 */
lh_void
lh_ui_point_set_along(lh_ui_point_t *self, lh_ui_axis_t axis, lh_ui_scalar_t value);

/**
 * @brief Fill @p self with @p along on @p axis and @p across on the other axis.
 */
lh_void
lh_ui_point_init_along(lh_ui_point_t *self, lh_ui_axis_t axis, lh_ui_scalar_t along, lh_ui_scalar_t across);

/**
 * @brief The extent of @p self on @p axis (width or height).
 */
lh_ui_scalar_t
lh_ui_size_get_along(const lh_ui_size_t *self, lh_ui_axis_t axis);

/**
 * @brief Replace the extent of @p self on @p axis with @p value.
 */
lh_void
lh_ui_size_set_along(lh_ui_size_t *self, lh_ui_axis_t axis, lh_ui_scalar_t value);

/**
 * @brief Fill @p self with the part of @p base on @p axis from @p start (past
 *        the origin of @p base) for @p length; across @p axis it is @p base.
 */
lh_void
lh_ui_rect_init_along(lh_ui_rect_t *self, const lh_ui_rect_t *base, lh_ui_axis_t axis, lh_ui_scalar_t start,
                      lh_ui_scalar_t length);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_AXIS_H */
