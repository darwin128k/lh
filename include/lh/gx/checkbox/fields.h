/**
 * @file fields.h
 * @brief Macro for declaring the members of ::lh_gx_checkbox_t.
 */

#ifndef LH_GX_CHECKBOX_FIELDS_H
#define LH_GX_CHECKBOX_FIELDS_H

/**
 * @def lh_gx_checkbox_fields(entity_type, style_type, change_fn, tap_type)
 * @brief The four entities of a checkbox, its two looks, its reaction and its
 *        state.
 *
 * - `row` — the checkbox itself: an entity of ::lh_gx_checkbox_class that
 *   draws nothing and takes input; its content points back at the checkbox;
 * - `square` — a box (::lh_gx_box_class) at the left of the row, as high as
 *   the row;
 * - `tick` — a smaller box inside the square, shown only while checked;
 * - `caption` — a text (::lh_gx_text_class) right of the square, centred
 *   in the row's height; its content is the string;
 * - `style` — the square's fill and border and the caption's ink and font;
 *   `tick_style` — the tick's fill;
 * - `on_change`, `context` — the application's reaction and its data;
 * - `tap` — the gesture on the row (::lh_gx_tap_t);
 * - `checked` — the state.
 *
 * None of the pointers owns: the styles are a theme's, the string, the
 * reaction and its context are the application's.
 *
 * @param entity_type Type of an entity (::lh_gx_entity_t).
 * @param style_type  Type of a style (::lh_gx_style_t).
 * @param change_fn   Type of the reaction (::lh_gx_checkbox_change_fn).
 * @param tap_type    Type of the gesture (::lh_gx_tap_t).
 */
#define lh_gx_checkbox_fields(entity_type, style_type, change_fn, tap_type)                        \
    entity_type row;                                                                               \
    entity_type square;                                                                            \
    entity_type tick;                                                                              \
    entity_type caption;                                                                           \
    const style_type *style;                                                                       \
    const style_type *tick_style;                                                                  \
    change_fn *on_change;                                                                          \
    lh_ptr context;                                                                                \
    tap_type tap;                                                                                  \
    lh_bool_t checked

#endif /* LH_GX_CHECKBOX_FIELDS_H */
