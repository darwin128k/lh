/**
 * @file fields.h
 * @brief Macro for declaring the members of ::lh_gx_canvas_t.
 */

#ifndef LH_GX_CANVAS_FIELDS_H
#define LH_GX_CANVAS_FIELDS_H

/**
 * @def lh_gx_canvas_fields(fill_rect_fn, fill_mask_fn)
 * @brief The slots of the contract: one pointer per call.
 *
 * Each parameter is a function type; the members are pointers to it. The
 * backend's context is not a field: whoever holds the table holds the context
 * beside it and passes it into every call.
 *
 * @param fill_rect_fn ::lh_gx_canvas_fill_rect_fn.
 * @param fill_mask_fn ::lh_gx_canvas_fill_mask_fn.
 */
#define lh_gx_canvas_fields(fill_rect_fn, fill_mask_fn)                                            \
    fill_rect_fn *fill_rect;                                                                       \
    fill_mask_fn *fill_mask

#endif /* LH_GX_CANVAS_FIELDS_H */
