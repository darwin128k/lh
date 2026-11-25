/**
 * @file fields.h
 * @brief Member fields of ::lh_gl_label_seen_t.
 */

#ifndef LH_GL_LABEL_SEEN_FIELDS_H
#define LH_GL_LABEL_SEEN_FIELDS_H

/**
 * @def lh_gl_label_seen_fields()
 * @brief What a label last saw of its text.
 *
 * The hash and the length of the text the label last showed, which is how
 * ::lh_gl_label_set_text_v tells a new string from the same one when the
 * caller has already overwritten the buffer the label points at, and `noted`,
 * where that text's ink was the last time the label recorded it, **relative to
 * the label's own origin**: the one way left to erase an old text whose bytes
 * are gone. A part of ::lh_gl_label_t that lives inside it
 * (::lh_gl_label_get_seen).
 */
#define lh_gl_label_seen_fields()                                                                  \
    lh_gl_rect_t noted;                                                                            \
    lh_u32_t hash;                                                                                 \
    lh_u32_t length;                                                                               \
    lh_bool_t hashed

#endif /* LH_GL_LABEL_SEEN_FIELDS_H */
