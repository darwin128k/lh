/**
 * @file fields.h
 * @brief Member fields of ::lh_ui_rects_t.
 */

#ifndef LH_UI_RECTS_FIELDS_H
#define LH_UI_RECTS_FIELDS_H

/**
 * @def lh_ui_rects_fields(rect_type, count_type)
 * @brief The rects of ::lh_ui_rects_t, and how many of them are in use.
 *
 * `rects` is the fixed table and is never reallocated, so a drawing path can
 * keep a region on its own stack and add to it per primitive. Entries from
 * `count` on hold nothing after ::lh_ui_rects_init.
 *
 * @param rect_type  ::lh_ui_rect_t.
 * @param count_type Unsigned type of the used count.
 */
#define lh_ui_rects_fields(rect_type, count_type)                                                      \
    rect_type rects[LH_UI_RECTS_MAX];                                                                  \
    count_type count

#endif /* LH_UI_RECTS_FIELDS_H */
