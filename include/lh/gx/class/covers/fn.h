/**
 * @file fn.h
 * @brief Function type ::lh_gx_class_covers_fn.
 */

#ifndef LH_GX_CLASS_COVERS_FN_H
#define LH_GX_CLASS_COVERS_FN_H

#include <lh/gx/rect.h>

struct lh_gx_entity;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @typedef lh_gx_class_covers_fn
 * @brief True when @p entity paints every pixel of @p area opaque.
 *
 * Optional, and asked only for an area inside the entity's rectangle. A renderer
 * that finds an entity covering the whole of the area it redraws starts from
 * that entity and draws nothing under it. Without this slot the entity never
 * covers anything — always correct, never the fastest.
 *
 * @param entity The entity.
 * @param place  Its rectangle on the screen.
 * @param area   The area being redrawn, inside @p place; screen coordinates.
 * @return True only when nothing under the entity can show through @p area.
 */
typedef lh_bool_t(lh_gx_class_covers_fn)(const struct lh_gx_entity *entity,
                                         const lh_gx_rect_t *place, const lh_gx_rect_t *area);

LH_COMPILER_EXTERN_C_END

#endif /* LH_GX_CLASS_COVERS_FN_H */
