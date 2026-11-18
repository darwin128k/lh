/**
 * @file fn.h
 * @brief Function type ::lh_gx_class_hit_fn.
 */

#ifndef LH_GX_CLASS_HIT_FN_H
#define LH_GX_CLASS_HIT_FN_H

#include <lh/gx/rect.h>

struct lh_gx_entity;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @typedef lh_gx_class_hit_fn
 * @brief True when the point @p at is on @p entity.
 *
 * Optional, and asked only for a point inside the entity's rectangle: the class
 * says where inside it the entity really is — a round button is not hit in its
 * corners, a ring not in its hole. Without this slot the whole rectangle is the
 * entity (::lh_gx_hit_is_on).
 *
 * @param entity The entity.
 * @param place  Its rectangle on the screen.
 * @param at     The point, screen coordinates, inside @p place.
 * @return True when the point is on the entity.
 */
typedef lh_bool_t(lh_gx_class_hit_fn)(const struct lh_gx_entity *entity, const lh_gx_rect_t *place,
                                      const lh_gx_point_t *at);

LH_COMPILER_EXTERN_C_END

#endif /* LH_GX_CLASS_HIT_FN_H */
