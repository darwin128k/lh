/**
 * @file fields.h
 * @brief Macro for declaring the members of ::lh_gx_drag_t.
 */

#ifndef LH_GX_DRAG_FIELDS_H
#define LH_GX_DRAG_FIELDS_H

/**
 * @def lh_gx_drag_fields(bool_type, point_type)
 * @brief The state of a drag: whether one is in progress, and where the
 *        pointer is.
 *
 * - `active` — whether a press on the entity is being dragged;
 * - `at` — where the pointer was at the last event of the drag, screen
 *   coordinates.
 *
 * @param bool_type  Type of the flag (::lh_bool_t).
 * @param point_type Type of a point (::lh_gx_point_t).
 */
#define lh_gx_drag_fields(bool_type, point_type)                                                   \
    bool_type active;                                                                              \
    point_type at

#endif /* LH_GX_DRAG_FIELDS_H */
