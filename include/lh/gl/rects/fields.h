/**
 * @file fields.h
 * @brief Member fields of ::lh_gl_rects_t.
 */

#ifndef LH_GL_RECTS_FIELDS_H
#define LH_GL_RECTS_FIELDS_H

/**
 * @def lh_gl_rects_fields(rect_type, count_type)
 * @brief The rects of ::lh_gl_rects_t, and how many of them are in use.
 *
 * `rects` is the fixed table and is never reallocated, so a drawing path can
 * keep a region on its own stack and add to it per primitive. Entries from
 * `count` on hold nothing after ::lh_gl_rects_init.
 *
 * @param rect_type  ::lh_gl_rect_t.
 * @param count_type Unsigned type of the used count.
 */
#define lh_gl_rects_fields(rect_type, count_type)                                                  \
    rect_type rects[LH_GL_RECTS_MAX];                                                              \
    count_type count

#endif /* LH_GL_RECTS_FIELDS_H */
