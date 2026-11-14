/**
 * @file layout.h
 * @brief One gap that places direct children: ::lh_gl_layout_t.
 *
 * The layout writes origins and nothing else. ::lh_gl_layout_pack walks the
 * layer's list from back to front, starting at zero. Along the chosen axis
 * each child is placed at the cursor, then the cursor advances by that child's
 * size plus the gap. The other axis and every size stay as they were. A second
 * pack starts at zero again.
 *
 * It does not draw, search or scroll. A container that shifts those origins
 * stays a separate object.
 */

#ifndef LH_GL_LAYOUT_H
#define LH_GL_LAYOUT_H

#include <lh/core/bool.h>
#include <lh/core/compiler/extern/c.h>
#include <lh/core/void.h>
#include <lh/gl/layer/decl.h>
#include <lh/gl/layout/fields.h>
#include <lh/gl/scalar.h>

/**
 * @struct lh_gl_layout
 * @brief The gap between packed children. Fields via ::lh_gl_layout_fields.
 */
struct lh_gl_layout
{
    /** Members: ::lh_gl_layout_fields. */
    lh_gl_layout_fields(lh_gl_scalar_t);
};
/** @copybrief lh_gl_layout */
typedef struct lh_gl_layout lh_gl_layout_t;

LH_COMPILER_EXTERN_C_BEGIN

/** @brief Gap added after each packed child. */
lh_gl_scalar_t
lh_gl_layout_get_gap(const lh_gl_layout_t *self);

/** @brief Store the gap. Negative pulls the next child back. */
lh_void
lh_gl_layout_set_gap(lh_gl_layout_t *self, lh_gl_scalar_t gap);

/** @brief Store @p gap. */
lh_void
lh_gl_layout_init(lh_gl_layout_t *self, lh_gl_scalar_t gap);

/**
 * @brief Place the direct children of @p layer along one axis.
 *
 * @p horizontal selects `x` and the child's width. Otherwise the axis is `y`
 * and the advance is the child's height. A null @p layer returns. List order
 * is the pack order. Sizes are not written.
 *
 * @param self       Layout; must not be null.
 * @param layer      Layer whose direct children move. May be null.
 * @param horizontal ::lh_bool_true packs across, ::lh_bool_false packs down.
 */
lh_void
lh_gl_layout_pack(const lh_gl_layout_t *self, lh_gl_layer_t *layer, lh_bool_t horizontal);

LH_COMPILER_EXTERN_C_END

#endif /* LH_GL_LAYOUT_H */
