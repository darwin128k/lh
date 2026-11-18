/**
 * @file fields.h
 * @brief Macro for declaring the members of ::lh_gx_focus_t.
 */

#ifndef LH_GX_FOCUS_FIELDS_H
#define LH_GX_FOCUS_FIELDS_H

/**
 * @def lh_gx_focus_fields(entity_type)
 * @brief Which entity the keys go to.
 *
 * `focused` is the entity with the focus, or ::lh_null for none; it is not
 * owned, and an entity that goes away must lose the focus first
 * (::lh_gx_focus_move to ::lh_null).
 *
 * @param entity_type Type of an entity (::lh_gx_entity_t).
 */
#define lh_gx_focus_fields(entity_type) entity_type *focused

#endif /* LH_GX_FOCUS_FIELDS_H */
