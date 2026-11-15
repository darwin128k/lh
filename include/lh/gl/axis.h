/**
 * @file axis.h
 * @brief One screen axis, ::lh_gl_axis_t, and geometry read and built along it.
 *
 * Code that works the same way across or down (a scrollbar, a list) takes an
 * axis and goes through these helpers instead of branching on x / y itself.
 */

#ifndef LH_GL_AXIS_H
#define LH_GL_AXIS_H

#include <lh/gl/rect.h>

/**
 * @enum lh_gl_axis
 * @brief Horizontal (`x`, width) or vertical (`y`, height).
 */
typedef enum lh_gl_axis
{
    lh_gl_axis_horizontal = 0, /**< `x` and width; left to right. */
    lh_gl_axis_vertical = 1    /**< `y` and height; top to bottom. */
} lh_gl_axis_t;                /**< Horizontal (`x`, width) or vertical (`y`, height). */

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The axis across @p axis: vertical for horizontal and back.
 */
lh_gl_axis_t
lh_gl_axis_get_cross(lh_gl_axis_t axis);

/**
 * @brief The coordinate of @p self on @p axis.
 */
lh_gl_scalar_t
lh_gl_point_get_along(const lh_gl_point_t *self, lh_gl_axis_t axis);

/**
 * @brief Replace the coordinate of @p self on @p axis with @p value.
 */
lh_void
lh_gl_point_set_along(lh_gl_point_t *self, lh_gl_axis_t axis, lh_gl_scalar_t value);

/**
 * @brief Fill @p self with @p along on @p axis and @p across on the other axis.
 */
lh_void
lh_gl_point_init_along(lh_gl_point_t *self, lh_gl_axis_t axis, lh_gl_scalar_t along,
                       lh_gl_scalar_t across);

/**
 * @brief The extent of @p self on @p axis (width or height).
 */
lh_gl_scalar_t
lh_gl_size_get_along(const lh_gl_size_t *self, lh_gl_axis_t axis);

/**
 * @brief Replace the extent of @p self on @p axis with @p value.
 */
lh_void
lh_gl_size_set_along(lh_gl_size_t *self, lh_gl_axis_t axis, lh_gl_scalar_t value);

/**
 * @brief Fill @p self with the part of @p base on @p axis from @p start (past
 *        the origin of @p base) for @p length; across @p axis it is @p base.
 */
lh_void
lh_gl_rect_init_along(lh_gl_rect_t *self, const lh_gl_rect_t *base, lh_gl_axis_t axis,
                      lh_gl_scalar_t start, lh_gl_scalar_t length);

LH_COMPILER_EXTERN_C_END

#endif /* LH_GL_AXIS_H */
