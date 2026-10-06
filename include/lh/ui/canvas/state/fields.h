/**
 * @file fields.h
 * @brief Member fields of ::lh_ui_canvas_state_t.
 */

#ifndef LH_UI_CANVAS_STATE_FIELDS_H
#define LH_UI_CANVAS_STATE_FIELDS_H

/**
 * @def lh_ui_canvas_state_fields(point_type, rect_type, bool_type)
 * @brief Where drawing lands and what it is cut to.
 *
 * `offset` is added to every primitive before the backend sees it. `clip` is
 * in target (screen) space, offset already applied; it means something only
 * when `clipped` is true.
 *
 * @param point_type Type of the offset.
 * @param rect_type  Type of the clip rectangle.
 * @param bool_type  Type of the clip flag.
 */
#define lh_ui_canvas_state_fields(point_type, rect_type, bool_type)                                 \
    point_type offset;                                                                              \
    rect_type clip;                                                                                 \
    bool_type clipped

#endif /* LH_UI_CANVAS_STATE_FIELDS_H */
