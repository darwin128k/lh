/**
 * @file fields.h
 * @brief Macro for declaring the members of ::lh_gx_present_t.
 */

#ifndef LH_GX_PRESENT_FIELDS_H
#define LH_GX_PRESENT_FIELDS_H

/**
 * @def lh_gx_present_fields(show_fn)
 * @brief The slots of the contract: one pointer per call.
 *
 * The target's context is not a field: whoever holds the table holds the
 * context beside it and passes it into every call, as with the canvas.
 *
 * @param show_fn ::lh_gx_present_show_fn.
 */
#define lh_gx_present_fields(show_fn) show_fn *show

#endif /* LH_GX_PRESENT_FIELDS_H */
