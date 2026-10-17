/**
 * @file fields.h
 * @brief Member fields of ::lh_ui_view_pointer_t.
 */

#ifndef LH_UI_VIEW_POINTER_FIELDS_H
#define LH_UI_VIEW_POINTER_FIELDS_H

/**
 * @def lh_ui_view_pointer_fields
 * @brief One pointer's press in a view, in this order:
 *
 * - `target` -- the entity the pointer went down on: it hears the release, and
 *   only a release over it again is a click;
 * - `drag` -- the container a content drag of this pointer moves; ::lh_null when
 *   there is none, or when another pointer already drags that container;
 * - `grab`, `grab_offset` -- the scrollbar whose thumb this pointer holds, and
 *   where on the thumb it holds it;
 * - `press_point`, `last_point`, `velocity` -- where the press went down, where
 *   the pointer was last seen, and how fast its drag goes (the glide its release
 *   starts);
 * - `pressed` -- a press is open; `dragged` -- it went past the threshold, so its
 *   release is no click.
 *
 * None of the pointers are owned.
 *
 * @param entity_type    ::lh_ui_entity_t.
 * @param container_type ::lh_ui_container_t.
 * @param scrollbar_type ::lh_ui_scrollbar_t.
 * @param scalar_type    ::lh_ui_scalar_t.
 * @param point_type     ::lh_ui_point_t.
 */
#define lh_ui_view_pointer_fields(entity_type, container_type, scrollbar_type, scalar_type,        \
                                  point_type)                                                      \
    entity_type *target;                                                                           \
    container_type *drag;                                                                          \
    scrollbar_type *grab;                                                                          \
    scalar_type grab_offset;                                                                       \
    point_type press_point;                                                                        \
    point_type last_point;                                                                         \
    point_type velocity;                                                                           \
    lh_bool_t pressed;                                                                             \
    lh_bool_t dragged

#endif /* LH_UI_VIEW_POINTER_FIELDS_H */
