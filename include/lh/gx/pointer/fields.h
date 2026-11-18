/**
 * @file fields.h
 * @brief Macro for declaring the members of ::lh_gx_pointer_t.
 */

#ifndef LH_GX_POINTER_FIELDS_H
#define LH_GX_POINTER_FIELDS_H

/**
 * @def lh_gx_pointer_fields(entity_type)
 * @brief The entity the gesture in progress belongs to.
 *
 * `captured` is the entity a press landed on, until the release or the cancel;
 * ::lh_null between gestures, and during a gesture that landed on nothing that
 * takes input. Not owned.
 *
 * @param entity_type Type of an entity (::lh_gx_entity_t).
 */
#define lh_gx_pointer_fields(entity_type) entity_type *captured

#endif /* LH_GX_POINTER_FIELDS_H */
