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
 * @def lh_ui_canvas_fields(backend_type, context_type, state_type, depth_type, size_type,
 *                          point_type, rect_type, bool_type, round_type, byte_type, usize_type)
 * @brief Backend table (not owned), the context passed into every call, the
 *        current offset / clip, the states saved by push, the target size, the
 *        accumulated damage rectangle, and the scratch for effects that read
 *        pixels.
 *
 * Capacity of `saved` is ::LH_LIBRARY_OPTION_UI_CANVAS_DEPTH; `depth` is how
 * many are in use. `rounds` holds the rounded cuts of the pushed clips, in
 * push order; the current state says how many are active. `size` is the whole target (for `clear` damage).
 * `frame_at` is the target-space origin of the buffer the current frame draws
 * into: `(0, 0)` on ::lh_ui_canvas_begin, the area origin on
 * ::lh_ui_canvas_begin_area. Primitives reach the backend already moved by it;
 * damage is put back, so `damage` unions every primitive rect already cut to
 * the clip in target space; empty when `has_damage` is false.
 *
 * `scratch` is the caller's memory for an effect that needs a second buffer of
 * its own (a blur is one), handed to every backend slot that asks for one. It is
 * not owned and never allocated: a drawing primitive that allocates is a drawing
 * primitive that needs a heap, and an app with no allocator under it would get a
 * fault instead of a "no". With no scratch the effect says it cannot be drawn.
 *
 * @param backend_type Type of the function table pointed at.
 * @param context_type Opaque backend state.
 * @param state_type   Type of one offset / clip level.
 * @param depth_type   Type of the push count.
 * @param size_type    Type of the target size.
 * @param point_type   Type of the frame buffer origin.
 * @param rect_type    Type of the damage rect.
 * @param bool_type    Type of the damage flag.
 * @param round_type   ::lh_ui_canvas_clip_round_t.
 * @param byte_type    Type of a byte of scratch.
 * @param usize_type   Type of a byte count.
 */
#define lh_ui_canvas_fields(backend_type, context_type, state_type, depth_type, size_type,          \
                            point_type, rect_type, bool_type, round_type, byte_type, usize_type)    \
    const backend_type *backend;                                                                    \
    context_type context;                                                                           \
    state_type state;                                                                               \
    state_type saved[LH_LIBRARY_OPTION_UI_CANVAS_DEPTH];                                            \
    depth_type depth;                                                                               \
    size_type size;                                                                                 \
    point_type frame_at;                                                                            \
    rect_type damage;                                                                               \
    bool_type has_damage;                                                                           \
    round_type rounds[LH_LIBRARY_OPTION_UI_CANVAS_DEPTH];                                           \
    byte_type *scratch;                                                                             \
    usize_type scratch_bytes

#endif /* LH_UI_CANVAS_FIELDS_H */
