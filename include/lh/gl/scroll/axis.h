/**
 * @file axis.h
 * @brief One scroll offset: ::lh_gl_scroll_axis_t.
 *
 * An axis is a single number. A horizontal scroll and a vertical scroll are
 * two axes. Neither is stored on an entity or a layer, and neither rewrites a
 * rectangle. The number is the whole value, so writing the axis out is writing
 * that number.
 *
 * Adding to the offset does not invalidate anything: the axis does not know
 * which layer it will shift. ::lh_gl_container_invalidate marks the window.
 */

#ifndef LH_GL_SCROLL_AXIS_H
#define LH_GL_SCROLL_AXIS_H

#include <lh/core/compiler/extern/c.h>
#include <lh/core/void.h>
#include <lh/gl/scalar.h>
#include <lh/gl/scroll/axis/fields.h>

/**
 * @struct lh_gl_scroll_axis
 * @brief One offset. Fields via ::lh_gl_scroll_axis_fields.
 */
struct lh_gl_scroll_axis
{
    /** Members: ::lh_gl_scroll_axis_fields. */
    lh_gl_scroll_axis_fields(lh_gl_scalar_t);
};
/** @copybrief lh_gl_scroll_axis */
typedef struct lh_gl_scroll_axis lh_gl_scroll_axis_t;

LH_COMPILER_EXTERN_C_BEGIN

/** @brief The stored offset. */
lh_gl_scalar_t
lh_gl_scroll_axis_get_offset(const lh_gl_scroll_axis_t *self);

/** @brief Store @p offset. */
lh_void
lh_gl_scroll_axis_set_offset(lh_gl_scroll_axis_t *self, lh_gl_scalar_t offset);

/** @brief Store @p offset. */
lh_void
lh_gl_scroll_axis_init(lh_gl_scroll_axis_t *self, lh_gl_scalar_t offset);

/**
 * @brief Add @p delta to the stored offset.
 *
 * The sum is unchecked, like any ::lh_gl_scalar_t addition. No rectangle
 * changes.
 *
 * @param self  Axis; must not be null.
 * @param delta Amount to add. Negative scrolls the other way.
 */
lh_void
lh_gl_scroll_axis_add(lh_gl_scroll_axis_t *self, lh_gl_scalar_t delta);

LH_COMPILER_EXTERN_C_END

#endif /* LH_GL_SCROLL_AXIS_H */
