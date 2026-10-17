/**
 * @file fields.h
 * @brief Member fields of ::lh_ui_present_sink_t.
 */

#ifndef LH_UI_PRESENT_SINK_FIELDS_H
#define LH_UI_PRESENT_SINK_FIELDS_H

/**
 * @def lh_ui_present_sink_fields(name_type, present_fn, context_type)
 * @brief One named place pixels go after a draw: name, present slot, context.
 *
 * The sink does not draw. It only shows what a pixmap already holds for the
 * damage list. Context is not owned.
 *
 * @param name_type    `const char *` (stable tag: `"gdi"`, `"web"`, `"f4ve"`).
 * @param present_fn   ::lh_ui_present_fn.
 * @param context_type ::lh_ptr.
 */
#define lh_ui_present_sink_fields(name_type, present_fn, context_type)                             \
    name_type name;                                                                                \
    present_fn *present;                                                                           \
    context_type context

#endif /* LH_UI_PRESENT_SINK_FIELDS_H */
