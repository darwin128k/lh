/**
 * @file fields.h
 * @brief Member fields of ::lh_gl_blur_window_t.
 */

#ifndef LH_GL_BLUR_WINDOW_FIELDS_H
#define LH_GL_BLUR_WINDOW_FIELDS_H

/**
 * @def lh_gl_blur_window_fields(sum_type)
 * @brief Four running sums. Red, green and blue are premultiplied by alpha.
 *        Alpha is the straight sum.
 *
 * @param sum_type ::lh_s32_t.
 */
#define lh_gl_blur_window_fields(sum_type)                                                         \
    sum_type r;                                                                                    \
    sum_type g;                                                                                    \
    sum_type b;                                                                                    \
    sum_type a

#endif /* LH_GL_BLUR_WINDOW_FIELDS_H */
