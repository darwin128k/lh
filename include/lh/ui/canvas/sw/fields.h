/**
 * @file fields.h
 * @brief Member fields of ::lh_ui_canvas_sw_t.
 */

#ifndef LH_UI_CANVAS_SW_FIELDS_H
#define LH_UI_CANVAS_SW_FIELDS_H

/**
 * @def lh_ui_canvas_sw_fields(pixmap_type, rect_type, clip_type)
 * @brief The pixmap drawn into, the limit every write is cut to, and the
 *        clip the canvas set (for its rounded cuts).
 *
 * `limit` is the pixmap bounds, intersected with the clip rect. `clip` has no
 * rounds when nothing is clipped; its rounds belong to the canvas.
 *
 * @param pixmap_type ::lh_ui_pixmap_t.
 * @param rect_type   ::lh_ui_rect_t.
 * @param clip_type   ::lh_ui_canvas_clip_t.
 */
#define lh_ui_canvas_sw_fields(pixmap_type, rect_type, clip_type)                                   \
    pixmap_type pixmap;                                                                             \
    rect_type limit;                                                                                \
    clip_type clip

#endif /* LH_UI_CANVAS_SW_FIELDS_H */
