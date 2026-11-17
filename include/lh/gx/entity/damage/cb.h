/**
 * @file cb.h
 * @brief Callable wrapper for the report of a changed entity.
 */

#ifndef LH_GX_ENTITY_DAMAGE_CB_H
#define LH_GX_ENTITY_DAMAGE_CB_H

#include <lh/core/util/ptr.h>
#include <lh/gx/entity/damage/fn.h>

/**
 * @def lh_gx_entity_damage_cb
 * @brief Pointer to ::lh_gx_entity_damage_fn.
 *
 * Expands to a function pointer type for the report of a changed entity.
 */
#define lh_gx_entity_damage_cb lh_ptr_of(lh_gx_entity_damage_fn)

#endif /* LH_GX_ENTITY_DAMAGE_CB_H */
