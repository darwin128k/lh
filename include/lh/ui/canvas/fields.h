/**
 * @file fields.h
 * @brief Member fields of ::lh_ui_canvas_t.
 */

#ifndef LH_UI_CANVAS_FIELDS_H
#define LH_UI_CANVAS_FIELDS_H

/**
 * @def lh_ui_canvas_fields(backend_type, context_type)
 * @brief Backend table (not owned) and the context passed into every call.
 *
 * @param backend_type Type of the function table pointed at.
 * @param context_type Opaque backend state.
 */
#define lh_ui_canvas_fields(backend_type, context_type)                                             \
    const backend_type *backend;                                                                    \
    context_type context

#endif /* LH_UI_CANVAS_FIELDS_H */
