/**
 * @file fields.h
 * @brief Member fields of ::lh_ui_effect_class_t.
 */

#ifndef LH_UI_EFFECT_CLASS_FIELDS_H
#define LH_UI_EFFECT_CLASS_FIELDS_H

/**
 * @def lh_ui_effect_class_fields(draw_fn, outset_fn)
 * @brief How an effect paints and how far it reaches outside its box.
 *
 * @param draw_fn   `void (*)(const effect *, canvas, box, corner)`.
 * @param outset_fn `int (*)(const effect *)`, pixels past the box.
 */
#define lh_ui_effect_class_fields(draw_fn, outset_fn)                                              \
    draw_fn draw;                                                                                  \
    outset_fn outset

#endif /* LH_UI_EFFECT_CLASS_FIELDS_H */
