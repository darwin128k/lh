/**
 * @file fields.h
 * @brief Member fields of ::lh_ui_canvas_t.
 */

#ifndef LH_UI_CANVAS_FIELDS_H
#define LH_UI_CANVAS_FIELDS_H

#include <lh/assert/static.h>
#include <lh/config.h>

/* The depth is one byte. */
lh_assert_static(LH_LIBRARY_OPTION_UI_CANVAS_DEPTH >= 1 && LH_LIBRARY_OPTION_UI_CANVAS_DEPTH <= 255,
                 "LH_LIBRARY_OPTION_UI_CANVAS_DEPTH must be in 1..255");

/**
 * @def lh_ui_canvas_fields(backend_type, context_type, state_type, depth_type, size_type, rect_type,
 *                          bool_type)
 * @brief Backend table (not owned), the context passed into every call, the
 *        current offset / clip, the states saved by push, the target size, and
 *        the accumulated damage rectangle.
 *
 * Capacity of `saved` is ::LH_LIBRARY_OPTION_UI_CANVAS_DEPTH; `depth` is how
 * many are in use. `size` is the whole target (for `clear` damage). `damage`
 * unions every primitive rect already cut to the clip; empty when
 * `has_damage` is false.
 *
 * @param backend_type Type of the function table pointed at.
 * @param context_type Opaque backend state.
 * @param state_type   Type of one offset / clip level.
 * @param depth_type   Type of the push count.
 * @param size_type    Type of the target size.
 * @param rect_type    Type of the damage rect.
 * @param bool_type    Type of the damage flag.
 */
#define lh_ui_canvas_fields(backend_type, context_type, state_type, depth_type, size_type,          \
                            rect_type, bool_type)                                                   \
    const backend_type *backend;                                                                    \
    context_type context;                                                                           \
    state_type state;                                                                               \
    state_type saved[LH_LIBRARY_OPTION_UI_CANVAS_DEPTH];                                            \
    depth_type depth;                                                                               \
    size_type size;                                                                                 \
    rect_type damage;                                                                               \
    bool_type has_damage

#endif /* LH_UI_CANVAS_FIELDS_H */
