/**
 * @file fields.h
 * @brief Macro for declaring the members of ::lh_gx_slider_t.
 */

#ifndef LH_GX_SLIDER_FIELDS_H
#define LH_GX_SLIDER_FIELDS_H

/**
 * @def lh_gx_slider_fields(entity_type, style_type, value_type, change_fn, drag_type)
 * @brief The four entities of a slider, its three looks, its range and value,
 *        its reaction and its gesture.
 *
 * - `row` — the slider itself: an entity of ::lh_gx_slider_class that draws
 *   nothing and takes input; its content points back at the slider;
 * - `rail` — a thin box across the row, from the knob's centre at the least
 *   value to its centre at the most;
 * - `fill` — a box over the rail, from its start to the knob's centre;
 * - `knob` — a box as high as the row and as wide, where the value is;
 * - `style` — the rail's look; `accent_style` — the fill's; `knob_style` —
 *   the knob's;
 * - `min`, `max` — the range, both included; `value` — where in it the
 *   slider is; `start` — the value a drag began from, put back on a cancel;
 * - `on_change`, `context` — the application's reaction and its data;
 * - `drag` — the gesture on the row (::lh_gx_drag_t).
 *
 * None of the pointers owns: the styles are a theme's, the reaction and its
 * context are the application's.
 *
 * @param entity_type Type of an entity (::lh_gx_entity_t).
 * @param style_type  Type of a style (::lh_gx_style_t).
 * @param value_type  Type of a value (::lh_s32_t).
 * @param change_fn   Type of the reaction (::lh_gx_slider_change_fn).
 * @param drag_type   Type of the gesture (::lh_gx_drag_t).
 */
#define lh_gx_slider_fields(entity_type, style_type, value_type, change_fn, drag_type)             \
    entity_type row;                                                                               \
    entity_type rail;                                                                              \
    entity_type fill;                                                                              \
    entity_type knob;                                                                              \
    const style_type *style;                                                                       \
    const style_type *accent_style;                                                                \
    const style_type *knob_style;                                                                  \
    value_type min;                                                                                \
    value_type max;                                                                                \
    value_type value;                                                                              \
    value_type start;                                                                              \
    change_fn *on_change;                                                                          \
    lh_ptr context;                                                                                \
    drag_type drag

#endif /* LH_GX_SLIDER_FIELDS_H */
