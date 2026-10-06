/**
 * @file fields.h
 * @brief Member fields of ::lh_ui_entity_container_t.
 */

#ifndef LH_UI_ENTITY_CONTAINER_FIELDS_H
#define LH_UI_ENTITY_CONTAINER_FIELDS_H

/**
 * @def lh_ui_entity_container_fields(entity_type, point_type)
 * @brief The entity this container is, then how far its content is scrolled.
 *
 * `scroll` is kept in `0 .. max` per axis, clamped on set and again on read
 * (::lh_ui_entity_container_clamp_scroll). Scrollbars are separate
 * components that point at the container; it keeps no fields for them.
 *
 * @param entity_type Type of the embedded entity.
 * @param point_type  Type of the scroll position.
 */
#define lh_ui_entity_container_fields(entity_type, point_type)                                      \
    entity_type entity;                                                                             \
    point_type scroll

#endif /* LH_UI_ENTITY_CONTAINER_FIELDS_H */
