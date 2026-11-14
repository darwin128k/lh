/**
 * @file scroll.h
 * @brief A shift of one layer's children: ::lh_gl_scroll_t.
 *
 * Scroll moves rectangles and nothing else. ::lh_gl_scroll_by adds a delta to
 * the origin of each direct child through ::lh_gl_entity_set_origin, so the
 * old and new areas are invalidated by that call. Children of those children
 * keep their own origins: a nested layer moves as one rectangle. The delta is
 * not stored. The rectangles are the position, and a later call adds again.
 *
 * There is no glide, no bar and no device.
 */

#ifndef LH_GL_SCROLL_H
#define LH_GL_SCROLL_H

#include <lh/gl/layer/decl.h>
#include <lh/gl/point.h>
#include <lh/gl/scroll/fields.h>

/**
 * @struct lh_gl_scroll
 * @brief Borrowed layer whose direct children scroll. Fields via ::lh_gl_scroll_fields.
 */
struct lh_gl_scroll
{
    /** Members: ::lh_gl_scroll_fields. */
    lh_gl_scroll_fields(lh_gl_layer_t);
};
/** @copybrief lh_gl_scroll */
typedef struct lh_gl_scroll lh_gl_scroll_t;

LH_COMPILER_EXTERN_C_BEGIN

/** @brief Borrowed layer, or ::lh_null. */
lh_gl_layer_t *
lh_gl_scroll_get_layer(const lh_gl_scroll_t *self);

/** @brief Store a borrowed layer. @p layer may be ::lh_null. */
lh_void
lh_gl_scroll_set_layer(lh_gl_scroll_t *self, lh_gl_layer_t *layer);

/** @brief Borrow @p layer. A null layer makes ::lh_gl_scroll_by do nothing. */
lh_void
lh_gl_scroll_init(lh_gl_scroll_t *self, lh_gl_layer_t *layer);

/**
 * @brief Add `(@p dx, @p dy)` to the origin of each direct child.
 *
 * A null layer or a zero delta returns without changing rectangles. Descendants
 * below the direct children are not visited.
 *
 * @param self Scroll state; must not be null.
 * @param dx Horizontal shift, added to each child's origin `x`.
 * @param dy Vertical shift, added to each child's origin `y`.
 */
lh_void
lh_gl_scroll_by(lh_gl_scroll_t *self, lh_math_scalar_t dx, lh_math_scalar_t dy);

LH_COMPILER_EXTERN_C_END

#endif /* LH_GL_SCROLL_H */
