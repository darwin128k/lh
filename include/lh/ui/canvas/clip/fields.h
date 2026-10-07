/**
 * @file fields.h
 * @brief Member fields of ::lh_ui_canvas_clip_round_t and ::lh_ui_canvas_clip_t.
 */

#ifndef LH_UI_CANVAS_CLIP_FIELDS_H
#define LH_UI_CANVAS_CLIP_FIELDS_H

/**
 * @def lh_ui_canvas_clip_round_fields(rect_type, scalar_type)
 * @brief One rounded cut: a rect in target space and its clamped corner radius.
 *
 * @param rect_type   ::lh_ui_rect_t.
 * @param scalar_type ::lh_ui_scalar_t.
 */
#define lh_ui_canvas_clip_round_fields(rect_type, scalar_type)                                      \
    rect_type rect;                                                                                 \
    scalar_type radius

/**
 * @def lh_ui_canvas_clip_fields(rect_type, round_type, count_type)
 * @brief The rect every primitive is cut to, and the rounded cuts on top of it.
 *
 * `rounds` is not owned: the canvas keeps them, valid until its clip changes.
 * A pixel is kept by the product of its coverage in every round.
 *
 * @param rect_type  ::lh_ui_rect_t.
 * @param round_type ::lh_ui_canvas_clip_round_t.
 * @param count_type Type of the round count.
 */
#define lh_ui_canvas_clip_fields(rect_type, round_type, count_type)                                 \
    rect_type rect;                                                                                 \
    const round_type *rounds;                                                                       \
    count_type round_count

#endif /* LH_UI_CANVAS_CLIP_FIELDS_H */
