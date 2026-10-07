/**
 * @file fields.h
 * @brief Member fields of ::lh_ui_canvas_sw_t.
 */

#ifndef LH_UI_CANVAS_SW_FIELDS_H
#define LH_UI_CANVAS_SW_FIELDS_H

/**
 * @def lh_ui_canvas_sw_fields(pixmap_type, rect_type)
 * @brief The pixmap drawn into and the limit every write is cut to.
 *
 * `limit` is the pixmap bounds, intersected with the clip the canvas set.
 *
 * @param pixmap_type ::lh_ui_pixmap_t.
 * @param rect_type   ::lh_ui_rect_t.
 */
#define lh_ui_canvas_sw_fields(pixmap_type, rect_type)                                              \
    pixmap_type pixmap;                                                                             \
    rect_type limit

#endif /* LH_UI_CANVAS_SW_FIELDS_H */
