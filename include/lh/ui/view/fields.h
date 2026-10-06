/**
 * @file fields.h
 * @brief Member fields of ::lh_ui_view_t.
 */

#ifndef LH_UI_VIEW_FIELDS_H
#define LH_UI_VIEW_FIELDS_H

#include <lh/bool.h>
#include <lh/ui/entity/scrollbar.h>
#include <lh/ui/scalar.h>

/**
 * @def lh_ui_view_fields(canvas_type, entity_type, color_type)
 * @brief Canvas, root entity, optional clear color, and pointer grab.
 *
 * None of the pointers are owned. Grab state is for thumb drag on a
 * scrollbar; click synthesis uses `pressed` / `dragged`.
 *
 * @param canvas_type Type of ::lh_ui_canvas_t.
 * @param entity_type Pointer type of ::lh_ui_entity_t.
 * @param color_type  Pointer type of ::lh_ui_color_t.
 */
#define lh_ui_view_fields(canvas_type, entity_type, color_type)                                     \
    canvas_type *canvas;                                                                            \
    entity_type *root;                                                                              \
    const color_type *clear;                                                                        \
    lh_ui_entity_scrollbar_t *grab;                                                                 \
    lh_ui_scalar_t grab_offset;                                                                     \
    lh_bool_t pressed;                                                                              \
    lh_bool_t dragged

#endif /* LH_UI_VIEW_FIELDS_H */
