/**
 * @file fields.h
 * @brief Macro for declaring the members of ::lh_gx_class_t.
 */

#ifndef LH_GX_CLASS_FIELDS_H
#define LH_GX_CLASS_FIELDS_H

/**
 * @def lh_gx_class_fields(paint_fn, covers_fn, hit_fn, input_fn, report_fn)
 * @brief The slots of the contract: one pointer per call.
 *
 * Each parameter is a function type; the members are pointers to it. A class
 * has no context of its own: every slot gets the entity, and the entity's
 * content is the class's data.
 *
 * @param paint_fn  ::lh_gx_class_paint_fn.
 * @param covers_fn ::lh_gx_class_covers_fn.
 * @param hit_fn    ::lh_gx_class_hit_fn.
 * @param input_fn  ::lh_gx_class_input_fn.
 * @param report_fn ::lh_gx_class_report_fn.
 */
#define lh_gx_class_fields(paint_fn, covers_fn, hit_fn, input_fn, report_fn)                       \
    paint_fn *paint;                                                                               \
    covers_fn *covers;                                                                             \
    hit_fn *hit;                                                                                   \
    input_fn *input;                                                                               \
    report_fn *report

#endif /* LH_GX_CLASS_FIELDS_H */
