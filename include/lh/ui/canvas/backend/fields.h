/**
 * @file fields.h
 * @brief Member fields of ::lh_ui_canvas_backend_t.
 */

#ifndef LH_UI_CANVAS_BACKEND_FIELDS_H
#define LH_UI_CANVAS_BACKEND_FIELDS_H

/**
 * @def lh_ui_canvas_backend_fields(begin_fn, begin_area_fn, end_fn, clear_fn, fill_rect_fn,
 *                                  fill_round_rect_fn, set_clip_fn, fill_mask_fn)
 * @brief Frame and 2D primitive entry points for one backend.
 *
 * Each parameter is a function type; the members are pointers to it. Context
 * is not a field: ::lh_ui_canvas_t holds it and passes it into every call.
 *
 * `begin_area` is optional: without it the canvas starts every frame through
 * `begin`, on the whole target. `fill_round_rect` is optional: without it, or
 * when it returns ::lh_bool_false, the canvas draws the rounded, anti-aliased
 * fill itself through `fill_rect`. `set_clip` is optional: without it the
 * canvas cuts every primitive to the clip itself. `fill_mask` is optional:
 * without it, or when it returns ::lh_bool_false, the canvas paints the mask
 * itself, pixel by pixel through `fill_rect` with the coverage in the alpha.
 */
#define lh_ui_canvas_backend_fields(begin_fn, begin_area_fn, end_fn, clear_fn, fill_rect_fn,          \
                                    fill_round_rect_fn, set_clip_fn, fill_mask_fn)                    \
    begin_fn *begin;                                                                                  \
    begin_area_fn *begin_area;                                                                        \
    end_fn *end;                                                                                      \
    clear_fn *clear;                                                                                  \
    fill_rect_fn *fill_rect;                                                                          \
    fill_round_rect_fn *fill_round_rect;                                                              \
    set_clip_fn *set_clip;                                                                            \
    fill_mask_fn *fill_mask

#endif /* LH_UI_CANVAS_BACKEND_FIELDS_H */
