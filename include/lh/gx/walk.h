/**
 * @file walk.h
 * @brief A walk of a tree in drawing order, with every entity's place on the
 *        screen: ::lh_gx_walk_t.
 *
 * Everything that goes over the tree goes in the same order — an owner before
 * its children, the children from the first (at the back) to the last (in
 * front) — and needs the same thing at every step: where the entity is on the
 * screen. The renderer walks to find the cover and to draw; the pointer walks to
 * find what a press landed on. This brick is that walk, once.
 *
 * It needs no stack and no recursion, so a tree of any depth is walked in a
 * fixed amount of memory: the tree has owner links, and a step up finds the
 * origin again from the owners (::lh_gx_entity_get_owner_origin). A step down
 * or across costs one addition.
 *
 * The caller decides at every entity whether to enter its children
 * (::lh_gx_walk_next): a hidden entity, or one that does not meet the area of
 * interest, is skipped with its whole subtree in one step. The walk itself
 * reads nothing but the links and the places; visibility and everything else is
 * the caller's.
 *
 * Usage:
 * @code
 * lh_gx_walk_t walk;
 *
 * lh_gx_walk_init(&walk, top);
 * do
 * {
 *     const lh_gx_entity_t *at = lh_gx_walk_get_entity(&walk);
 *     lh_bool_t descend = ...;   // look at at and lh_gx_walk_get_place(&walk)
 * } while (lh_gx_walk_next(&walk, descend) != lh_gx_walk_step_end);
 * @endcode
 *
 * Ownership: the walk owns nothing. The tree must not change while it is
 * walked.
 */

#ifndef LH_GX_WALK_H
#define LH_GX_WALK_H

#include <lh/gx/entity.h>
#include <lh/gx/walk/fields.h>
#include <lh/gx/walk/step.h>

/**
 * @struct lh_gx_walk
 * @brief Where a walk is in the tree, and that entity's place on the screen.
 */
struct lh_gx_walk
{
    /** Members: ::lh_gx_walk_fields. */
    lh_gx_walk_fields(lh_gx_entity_t, lh_gx_point_t, lh_gx_rect_t);
};
/** @copybrief lh_gx_walk */
typedef struct lh_gx_walk lh_gx_walk_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The entity whose subtree is walked (level 1).
 *
 * @param self The walk; checked by ::lh_assert_runtime_ref.
 */
const lh_gx_entity_t *
lh_gx_walk_get_top(const lh_gx_walk_t *self);

/**
 * @brief Store the entity whose subtree is walked (level 1).
 *
 * @param self The walk; checked by ::lh_assert_runtime_ref.
 * @param top  The top; not owned.
 */
lh_void
lh_gx_walk_set_top(lh_gx_walk_t *self, const lh_gx_entity_t *top);

/**
 * @brief The entity the walk is at (level 1).
 *
 * @param self The walk; checked by ::lh_assert_runtime_ref.
 * @return The entity, or ::lh_null when the walk is over.
 */
const lh_gx_entity_t *
lh_gx_walk_get_entity(const lh_gx_walk_t *self);

/**
 * @brief Store the entity the walk is at (level 1); its place is not computed.
 *
 * @param self   The walk; checked by ::lh_assert_runtime_ref.
 * @param entity The entity, or ::lh_null.
 */
lh_void
lh_gx_walk_set_entity(lh_gx_walk_t *self, const lh_gx_entity_t *entity);

/**
 * @brief The screen position of the current entity's owner (level 1).
 *
 * @param self The walk; checked by ::lh_assert_runtime_ref.
 * @return The point, inside @p self.
 */
const lh_gx_point_t *
lh_gx_walk_get_origin(const lh_gx_walk_t *self);

/**
 * @brief Store the screen position of the current entity's owner (level 1).
 *
 * @param self   The walk; checked by ::lh_assert_runtime_ref.
 * @param origin The point.
 */
lh_void
lh_gx_walk_set_origin(lh_gx_walk_t *self, lh_gx_point_t origin);

/**
 * @brief The current entity's rectangle on the screen (level 1).
 *
 * @param self The walk; checked by ::lh_assert_runtime_ref.
 * @return The rectangle, inside @p self.
 */
const lh_gx_rect_t *
lh_gx_walk_get_place(const lh_gx_walk_t *self);

/**
 * @brief Store the current entity's rectangle on the screen (level 1).
 *
 * @param self  The walk; checked by ::lh_assert_runtime_ref.
 * @param place The rectangle.
 */
lh_void
lh_gx_walk_set_place(lh_gx_walk_t *self, lh_gx_rect_t place);

/**
 * @brief Make @p entity the current one, its place counted from the walk's
 *        origin.
 *
 * The origin must already be that of @p entity's owner.
 *
 * @param self   The walk; must not be null.
 * @param entity The entity; must not be null.
 */
lh_void
lh_gx_walk_enter(lh_gx_walk_t *self, const lh_gx_entity_t *entity);

/**
 * @brief A walk of the subtree of @p top, at @p top.
 *
 * @param self The walk; must not be null.
 * @param top  The top; must not be null. Its own place is counted from the
 *             screen position of its owner, if it has one.
 */
lh_void
lh_gx_walk_init(lh_gx_walk_t *self, const lh_gx_entity_t *top);

/**
 * @brief Go to the next entity in drawing order: the first child of the current
 *        one when @p descend is true and it has one, else the next entity after
 *        its subtree.
 *
 * @param self    The walk; must not be over.
 * @param descend Whether to enter the current entity's children.
 * @return Which way the walk went; ::lh_gx_walk_step_end when it is over — then
 *         ::lh_gx_walk_get_entity is ::lh_null.
 */
lh_gx_walk_step_t
lh_gx_walk_next(lh_gx_walk_t *self, lh_bool_t descend);

LH_COMPILER_EXTERN_C_END

#endif /* LH_GX_WALK_H */
