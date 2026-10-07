/**
 * @file fields.h
 * @brief Member fields of ::lh_ui_view_t.
 */

#ifndef LH_UI_VIEW_FIELDS_H
#define LH_UI_VIEW_FIELDS_H

#include <lh/bool.h>
#include <lh/ui/entity/scrollbar.h>
#include <lh/ui/point.h>
#include <lh/ui/scalar.h>

/**
 * @def lh_ui_view_fields(canvas_type, entity_type, color_type)
 * @brief Canvas, root entity, optional clear color, partial strip height,
 *        pointer session, scroll gesture, throw, and keyboard focus.
 *
 * None of the pointers are owned. `strip_height`: rows per buffer when the
 * frame is drawn strip by strip, `0` for one whole-target frame.
 * `grab` / `grab_offset`: a dragged thumb. `pressed` / `dragged`: click
 * synthesis. `target`: the entity the pointer went down on (gets release).
 * `drag`: the container a content drag would move; `press_point` /
 * `last_point` / `velocity` track it. `scrolling`: the container whose gesture
 * began (scroll_begin sent, scroll_end pending). `throwing`: the container
 * gliding after a release. `focus`: keys go here.
 *
 * @param canvas_type Type of ::lh_ui_canvas_t.
 * @param entity_type Pointer type of ::lh_ui_entity_t.
 * @param color_type  Pointer type of ::lh_ui_color_t.
 */
#define lh_ui_view_fields(canvas_type, entity_type, color_type)                                     \
    canvas_type *canvas;                                                                            \
    entity_type *root;                                                                              \
    const color_type *clear;                                                                        \
    lh_ui_scalar_t strip_height;                                                                    \
    lh_ui_entity_scrollbar_t *grab;                                                                 \
    lh_ui_scalar_t grab_offset;                                                                     \
    lh_bool_t pressed;                                                                              \
    lh_bool_t dragged;                                                                              \
    entity_type *target;                                                                            \
    lh_ui_entity_container_t *drag;                                                                 \
    lh_ui_point_t press_point;                                                                      \
    lh_ui_point_t last_point;                                                                       \
    lh_ui_point_t velocity;                                                                         \
    lh_ui_entity_container_t *scrolling;                                                            \
    lh_ui_entity_container_t *throwing;                                                             \
    entity_type *focus

#endif /* LH_UI_VIEW_FIELDS_H */
