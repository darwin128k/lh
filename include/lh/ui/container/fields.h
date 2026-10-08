/**
 * @file fields.h
 * @brief Member fields of ::lh_ui_entity_container_t.
 */

#ifndef LH_UI_ENTITY_CONTAINER_FIELDS_H
#define LH_UI_ENTITY_CONTAINER_FIELDS_H

/**
 * @def lh_ui_entity_container_fields(entity_type, point_type, layout_type)
 * @brief The entity this container is, how far its content is scrolled, and the
 *        flow its children are placed by.
 *
 * `scroll` is kept in `0 .. max` per axis, clamped on set and again on read
 * (::lh_ui_entity_container_clamp_scroll). Scrollbars are separate
 * components that point at the container; it keeps no fields for them.
 *
 * `layout` is the flow, **not owned**, and `lh_null` for none — the same rule
 * as a style: a container nobody gave a flow to keeps its children exactly where
 * they were put, and a default flow would silently move them (measured: fourteen
 * tests that place children by hand went red on a default that nobody asked for).
 * When there is one, ::lh_ui_entity_container_place_children applies it before it
 * hands the transform on, which is why a container that moved carries its
 * children with it and the app never places anything by hand.
 *
 * @param entity_type Type of the embedded entity.
 * @param point_type  Type of the scroll position.
 * @param layout_type Type of the flow.
 */
#define lh_ui_entity_container_fields(entity_type, point_type, layout_type)                           \
    entity_type entity;                                                                             \
    point_type scroll;                                                                               \
    const layout_type *layout

#endif /* LH_UI_ENTITY_CONTAINER_FIELDS_H */