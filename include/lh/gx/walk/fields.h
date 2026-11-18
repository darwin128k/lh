/**
 * @file fields.h
 * @brief Macro for declaring the members of ::lh_gx_walk_t.
 */

#ifndef LH_GX_WALK_FIELDS_H
#define LH_GX_WALK_FIELDS_H

/**
 * @def lh_gx_walk_fields(entity_type, point_type, rect_type)
 * @brief Where a walk of a tree is, and where that is on the screen.
 *
 * `top` is the entity whose subtree is walked; `entity` the one the walk is at
 * (::lh_null when it is over); `origin` the screen position of that entity's
 * owner — where its place is counted from; `place` the entity's rectangle on the
 * screen. None of the pointers owns: a walk lives on its caller's stack.
 *
 * @param entity_type Type of an entity (::lh_gx_entity_t).
 * @param point_type  Type of the origin (::lh_gx_point_t).
 * @param rect_type   Type of the place (::lh_gx_rect_t).
 */
#define lh_gx_walk_fields(entity_type, point_type, rect_type)                                      \
    const entity_type *top;                                                                        \
    const entity_type *entity;                                                                     \
    point_type origin;                                                                             \
    rect_type place

#endif /* LH_GX_WALK_FIELDS_H */
