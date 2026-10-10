/**
 * @file fields.h
 * @brief Member fields of ::lh_ui_view_t.
 */

#ifndef LH_UI_VIEW_FIELDS_H
#define LH_UI_VIEW_FIELDS_H

#include <lh/bool.h>
#include <lh/ui/scrollbar.h>
#include <lh/ui/point.h>
#include <lh/ui/scalar.h>

/**
 * @def LH_UI_VIEW_WHOLE_MAX
 * @brief How many whole areas one view holds: regions a frame reads pixels of.
 *
 * A blur has to find the pixels it is about to blur, and in a frame drawn strip
 * by strip the pixels across the strip line are not in the buffer. The caller
 * names such a region here and the strips that touch it are drawn wider, so the
 * region arrives whole. It is a structural limit, not a knob: a frame that needs
 * more than this many regions is a frame whose areas have stopped being local.
 */
#define LH_UI_VIEW_WHOLE_MAX 4

/**
 * @def lh_ui_view_fields(canvas_type, entity_type, color_type, rect_type)
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
 * `whole`: regions a frame reads pixels of (::lh_ui_view_add_whole_area), and
 * `whole_count` how many are in use.
 * `pending` is the one rect a setter, a press or a scroll has asked the window
 * to paint again (::lh_ui_view_add_damage). `has_pending` is false when nothing
 * has, and ::lh_ui_view_take_damage hands the rect to the window and clears it.
 * It is not the canvas damage: that one is what the frame just drew, and the
 * frame resets it.
 *
 * @param canvas_type Type of ::lh_ui_canvas_t.
 * @param entity_type Pointer type of ::lh_ui_entity_t.
 * @param color_type  Pointer type of ::lh_ui_color_t.
 * @param rect_type   Type of ::lh_ui_rect_t.
 */
#define lh_ui_view_fields(canvas_type, entity_type, color_type, rect_type)                         \
    canvas_type *canvas;                                                                            \
    entity_type *root;                                                                              \
    const color_type *clear;                                                                        \
    lh_ui_scalar_t strip_height;                                                                    \
    lh_ui_scrollbar_t *grab;                                                                 \
    lh_ui_scalar_t grab_offset;                                                                     \
    lh_bool_t pressed;                                                                              \
    lh_bool_t dragged;                                                                              \
    entity_type *target;                                                                            \
    lh_ui_container_t *drag;                                                                 \
    lh_ui_point_t press_point;                                                                      \
    lh_ui_point_t last_point;                                                                       \
    lh_ui_point_t velocity;                                                                         \
    lh_ui_container_t *scrolling;                                                            \
    lh_ui_container_t *throwing;                                                             \
    entity_type *focus;                                                                             \
    rect_type whole[LH_UI_VIEW_WHOLE_MAX];                                                          \
    lh_ui_scalar_t whole_count;                                                                     \
    rect_type pending;                                                                              \
    lh_bool_t has_pending

#endif /* LH_UI_VIEW_FIELDS_H */
