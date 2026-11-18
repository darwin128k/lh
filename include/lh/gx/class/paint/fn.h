/**
 * @file fn.h
 * @brief Function type ::lh_gx_class_paint_fn.
 */

#ifndef LH_GX_CLASS_PAINT_FN_H
#define LH_GX_CLASS_PAINT_FN_H

#include <lh/core/compiler/extern/c.h>
#include <lh/core/ptr.h>

struct lh_gx_entity;
struct lh_gx_paint;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @typedef lh_gx_class_paint_fn
 * @brief Draw @p entity with @p paint.
 *
 * Optional: an entity whose class has no `paint` draws nothing of its own and
 * only holds its children — a layer. The paint carries the canvas, the clip and
 * the entity's rectangle on the screen (::lh_gx_paint_t); the class draws through
 * it and never sees the clip.
 *
 * @param entity The entity being drawn.
 * @param paint  Where and with what it draws.
 */
typedef lh_void(lh_gx_class_paint_fn)(const struct lh_gx_entity *entity,
                                      const struct lh_gx_paint *paint);

LH_COMPILER_EXTERN_C_END

#endif /* LH_GX_CLASS_PAINT_FN_H */
