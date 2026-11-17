/**
 * @file fn.h
 * @brief Callable signature for the report of a changed entity.
 *
 * An entity never remembers that it is invalid. When something of it changes,
 * it reports where it was and where it is now, and whoever listens — the
 * canvas — remembers that those places need painting again.
 */

#ifndef LH_GX_ENTITY_DAMAGE_FN_H
#define LH_GX_ENTITY_DAMAGE_FN_H

#include <lh/core/ptr.h>
#include <lh/gx/rect.h>

struct lh_gx_entity;

/**
 * @typedef lh_gx_entity_damage_fn
 * @brief Function type `lh_void(lh_ptr, const lh_gx_rect_t *, const lh_gx_rect_t *)`
 *        for the report of a changed entity.
 *
 * @p before is where the entity was, @p after is where it is now. The two are
 * the same when only a state changed and the entity did not move. A listener
 * that collects damage adds both.
 *
 * @param context The listener's own state.
 * @param before  Where the entity was; must not be null.
 * @param after   Where the entity is now; must not be null.
 */
typedef lh_void(lh_gx_entity_damage_fn)(lh_ptr context, const lh_gx_rect_t *before,
                                         const lh_gx_rect_t *after);

#endif /* LH_GX_ENTITY_DAMAGE_FN_H */
