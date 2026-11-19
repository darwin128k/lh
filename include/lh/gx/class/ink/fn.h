/**
 * @file fn.h
 * @brief Function type ::lh_gx_class_ink_fn.
 */

#ifndef LH_GX_CLASS_INK_FN_H
#define LH_GX_CLASS_INK_FN_H

#include <lh/gx/rect.h>

struct lh_gx_entity;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @typedef lh_gx_class_ink_fn
 * @brief Where @p entity draws when it stands at @p place.
 *
 * Optional; without it an entity draws inside its place. A class whose paint
 * reaches past the place — a box's shadow — says how far, and the renderer paints
 * the entity over that rectangle (its children still inside the place), and a
 * change of the entity is reported over it, so what it drew outside is not cut
 * and not left behind (::lh_gx_entity_get_ink).
 *
 * @param entity The entity.
 * @param place  Its rectangle, in the coordinates @p out is wanted in.
 * @param out    Receives the rectangle it draws in, @p place included.
 */
typedef lh_void(lh_gx_class_ink_fn)(const struct lh_gx_entity *entity, const lh_gx_rect_t *place,
                                    lh_gx_rect_t *out);

LH_COMPILER_EXTERN_C_END

#endif /* LH_GX_CLASS_INK_FN_H */
