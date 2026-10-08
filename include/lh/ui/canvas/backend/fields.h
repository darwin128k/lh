/**
 * @file fields.h
 * @brief Member fields of ::lh_ui_canvas_backend_t.
 */

#ifndef LH_UI_CANVAS_BACKEND_FIELDS_H
#define LH_UI_CANVAS_BACKEND_FIELDS_H

/**
 * @def lh_ui_canvas_backend_fields(begin_fn, begin_area_fn, end_fn, clear_fn, fill_rect_fn,
 *                                  fill_round_rect_fn, set_clip_fn, fill_mask_fn, shadow_fn,
 *                                  blur_fn, glass_fn)
 * @brief Frame, 2D primitive and effect entry points for one backend.
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
 *
 * `shadow`, `blur` and `glass` are the effects, and optional in a stronger
 * sense than the primitives: `shadow` falls back the way `fill_mask` does, but
 * `blur` and `glass` read pixels rather than write them, so a backend without
 * those slots cannot draw them and the canvas reports that instead of
 * substituting something that only looks like the effect.
 *
 * @param begin_fn        ::lh_ui_canvas_begin_fn.
 * @param begin_area_fn   ::lh_ui_canvas_begin_area_fn.
 * @param end_fn          ::lh_ui_canvas_end_fn.
 * @param clear_fn        ::lh_ui_canvas_clear_fn.
 * @param fill_rect_fn    ::lh_ui_canvas_fill_rect_fn.
 * @param fill_round_rect_fn ::lh_ui_canvas_fill_round_rect_fn.
 * @param set_clip_fn     ::lh_ui_canvas_set_clip_fn.
 * @param fill_mask_fn    ::lh_ui_canvas_fill_mask_fn.
 * @param shadow_fn       ::lh_ui_canvas_shadow_fn.
 * @param blur_fn         ::lh_ui_canvas_blur_fn.
 * @param glass_fn        ::lh_ui_canvas_glass_fn.
 */
#define lh_ui_canvas_backend_fields(begin_fn, begin_area_fn, end_fn, clear_fn, fill_rect_fn,          \
                                    fill_round_rect_fn, set_clip_fn, fill_mask_fn, shadow_fn, blur_fn, \
                                    glass_fn)                                                            \
    begin_fn *begin;                                                                                  \
    begin_area_fn *begin_area;                                                                        \
    end_fn *end;                                                                                      \
    clear_fn *clear;                                                                                  \
    fill_rect_fn *fill_rect;                                                                          \
    fill_round_rect_fn *fill_round_rect;                                                              \
    set_clip_fn *set_clip;                                                                            \
    fill_mask_fn *fill_mask;                                                                          \
    shadow_fn *shadow;                                                                                \
    blur_fn *blur;                                                                                    \
    glass_fn *glass

#endif /* LH_UI_CANVAS_BACKEND_FIELDS_H */
