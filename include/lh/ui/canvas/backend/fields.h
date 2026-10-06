/**
 * @file fields.h
 * @brief Member fields of ::lh_ui_canvas_backend_t.
 */

#ifndef LH_UI_CANVAS_BACKEND_FIELDS_H
#define LH_UI_CANVAS_BACKEND_FIELDS_H

/**
 * @def lh_ui_canvas_backend_fields(begin_fn, end_fn, clear_fn, fill_rect_fn)
 * @brief Frame and 2D primitive entry points for one backend.
 *
 * Each parameter is a function type; the members are pointers to it. Context
 * is not a field: ::lh_ui_canvas_t holds it and passes it into every call.
 */
#define lh_ui_canvas_backend_fields(begin_fn, end_fn, clear_fn, fill_rect_fn)                       \
    begin_fn *begin;                                                                                \
    end_fn *end;                                                                                    \
    clear_fn *clear;                                                                                \
    fill_rect_fn *fill_rect

#endif /* LH_UI_CANVAS_BACKEND_FIELDS_H */
