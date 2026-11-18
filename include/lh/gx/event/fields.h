/**
 * @file fields.h
 * @brief Macro for declaring the members of ::lh_gx_event_t.
 */

#ifndef LH_GX_EVENT_FIELDS_H
#define LH_GX_EVENT_FIELDS_H

/**
 * @def lh_gx_event_fields(kind_type, point_type)
 * @brief What happened to the pointer, and where.
 *
 * `kind` is what happened (::lh_gx_event_kind_t); `at` is where the pointer is,
 * in screen coordinates — the same as an entity's rectangle on the screen
 * (::lh_gx_entity_get_screen_rect), so a class compares the two directly.
 *
 * @param kind_type  Type of the kind (::lh_gx_event_kind_t).
 * @param point_type Type of the point (::lh_gx_point_t).
 */
#define lh_gx_event_fields(kind_type, point_type)                                                  \
    kind_type kind;                                                                                \
    point_type at

#endif /* LH_GX_EVENT_FIELDS_H */
