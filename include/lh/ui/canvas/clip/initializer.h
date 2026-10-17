/**
 * @file initializer.h
 * @brief Brace-enclosed initializer macros for ::lh_ui_canvas_clip_round_t and
 *        ::lh_ui_canvas_clip_t.
 */

#ifndef LH_UI_CANVAS_CLIP_INITIALIZER_H
#define LH_UI_CANVAS_CLIP_INITIALIZER_H

#include <lh/initializer.h>

/**
 * @def lh_ui_canvas_clip_round_initializer(rect, radius)
 * @brief Produces a brace-enclosed initializer for ::lh_ui_canvas_clip_round_t.
 *
 * Expands to ::lh_initializer with the arguments in the field order of
 * ::lh_ui_canvas_clip_round_fields.
 *
 * @param rect   Initializer of the ::lh_ui_rect_t of the cut, in target space.
 * @param radius Clamped corner radius.
 *
 * Example usage:
 * @code{.c}
 * static const lh_ui_canvas_clip_round_t round = lh_ui_canvas_clip_round_initializer(
 *     lh_initializer(lh_initializer(0, 0), lh_initializer(100, 50)),  // rect
 *     8);                                                             // radius
 * @endcode
 *
 * @see lh_initializer
 * @see lh_ui_canvas_clip_round_t
 */
#define lh_ui_canvas_clip_round_initializer(rect, radius) lh_initializer(rect, radius)

/**
 * @def lh_ui_canvas_clip_initializer(rect, rounds, round_count)
 * @brief Produces a brace-enclosed initializer for ::lh_ui_canvas_clip_t.
 *
 * Expands to ::lh_initializer with the arguments in the field order of
 * ::lh_ui_canvas_clip_fields.
 *
 * @param rect        Initializer of the ::lh_ui_rect_t every primitive is cut to.
 * @param rounds      `const lh_ui_canvas_clip_round_t *` (not owned), or ::lh_null.
 * @param round_count How many rounds @p rounds points to.
 *
 * Example usage:
 * @code{.c}
 * const lh_ui_canvas_clip_t clip = lh_ui_canvas_clip_initializer(
 *     lh_initializer(lh_initializer(0, 0), lh_initializer(100, 50)),  // rect
 *     &round,                                                         // rounds
 *     1);                                                             // round_count
 * @endcode
 *
 * @see lh_initializer
 * @see lh_ui_canvas_clip_t
 */
#define lh_ui_canvas_clip_initializer(rect, rounds, round_count)                                   \
    lh_initializer(rect, rounds, round_count)

#endif /* LH_UI_CANVAS_CLIP_INITIALIZER_H */
