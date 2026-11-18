/**
 * @file hit.h
 * @brief What is under a point of the screen: ::lh_gx_hit_find.
 *
 * Two bricks, the second built on the first:
 *
 * - ::lh_gx_hit_is_on — is a point on one entity: inside its rectangle, and
 *   where its class says it is (`hit` of ::lh_gx_class_t; without it, the whole
 *   rectangle);
 * - ::lh_gx_hit_find — the entity a press at a point goes to: the front-most
 *   one that shows, takes input (::lh_gx_entity_can_take_input) and is hit.
 *
 * The search is one walk of the tree in drawing order (::lh_gx_walk_t), and the
 * last match wins — what is drawn last is in front. It enters only the subtrees
 * whose owner holds the point: a child is drawn cut by its owner, so where the
 * owner is not, no child can be seen, nor hit. A hidden entity is skipped with
 * its subtree, as the renderer skips it.
 */

#ifndef LH_GX_HIT_H
#define LH_GX_HIT_H

#include <lh/gx/entity.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief True when @p at is on @p entity, whose rectangle on the screen is
 *        @p place: inside @p place and, if its class fills `hit`, where the class
 *        says.
 *
 * @param entity The entity; must not be null.
 * @param place  Its rectangle on the screen.
 * @param at     The point, screen coordinates.
 */
lh_bool_t
lh_gx_hit_is_on(const lh_gx_entity_t *entity, const lh_gx_rect_t *place, const lh_gx_point_t *at);

/**
 * @brief The front-most entity of the subtree of @p top that shows, takes input
 *        and is hit at @p at.
 *
 * @param top The top of the search, usually the screen; must not be null. The
 *            whole subtree is the caller's, so what is found is returned
 *            mutable.
 * @param at  The point, screen coordinates.
 * @return The entity, or ::lh_null when nothing that takes input is there.
 */
lh_gx_entity_t *
lh_gx_hit_find(lh_gx_entity_t *top, const lh_gx_point_t *at);

LH_COMPILER_EXTERN_C_END

#endif /* LH_GX_HIT_H */
