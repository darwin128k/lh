/**
 * @file fields.h
 * @brief Member fields of ::lh_gl_web_host_stream_t.
 */

#ifndef LH_GL_WEB_HOST_STREAM_FIELDS_H
#define LH_GL_WEB_HOST_STREAM_FIELDS_H

/**
 * @def lh_gl_web_host_stream_fields(glyphs_type, text_type, size_type)
 * @brief What the stream of WS frames shares between iterations:
 *
 * - `glyphs` — covers every synced client already holds;
 * - `last`, `last_len`, `last_cap` — the bytes of the last frame sent (owned), how
 *   many, and the capacity.
 */
#define lh_gl_web_host_stream_fields(glyphs_type, text_type, size_type)                            \
    glyphs_type glyphs;                                                                            \
    text_type last;                                                                                \
    size_type last_len;                                                                            \
    size_type last_cap

#endif /* LH_GL_WEB_HOST_STREAM_FIELDS_H */
