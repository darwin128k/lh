/**
 * @file fields.h
 * @brief Member fields of ::lh_ui_effect_t.
 */

#ifndef LH_UI_EFFECT_FIELDS_H
#define LH_UI_EFFECT_FIELDS_H

/**
 * @def lh_ui_effect_fields(class_type, effect_type)
 * @brief The head of every effect. A derived effect pastes this first, so a
 *        pointer to it is a pointer to the effect.
 *
 * - `class`: how this effect paints. Not owned.
 * - `next`: another effect drawn after this one, or null. Not owned.
 *
 * @param class_type  `const lh_ui_effect_class_t *`.
 * @param effect_type `const lh_ui_effect_t *`.
 */
#define lh_ui_effect_fields(class_type, effect_type)                                               \
    class_type class_ptr;                                                                          \
    effect_type next

#endif /* LH_UI_EFFECT_FIELDS_H */
