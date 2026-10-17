/**
 * @file initializer.h
 * @brief Brace-enclosed initializer macro for ::lh_ui_pixmap_t.
 */

#ifndef LH_UI_PIXMAP_INITIALIZER_H
#define LH_UI_PIXMAP_INITIALIZER_H

#include <lh/initializer.h>

/**
 * @def lh_ui_pixmap_initializer(bits, width, height, stride, format)
 * @brief Produces a brace-enclosed initializer for ::lh_ui_pixmap_t.
 *
 * Expands to ::lh_initializer with the arguments in the field order of
 * ::lh_ui_pixmap_fields.
 *
 * @param bits   Pixel bytes (not owned), or ::lh_null.
 * @param width  Width in pixels.
 * @param height Height in pixels.
 * @param stride Bytes from one row to the next.
 * @param format Pixel format (::lh_ui_pixmap_format_t).
 *
 * Example usage:
 * @code{.c}
 * static lh_byte_t pixels[4 * 4 * 2];
 * const lh_ui_pixmap_t pixmap = lh_ui_pixmap_initializer(
 *     pixels,                         // bits
 *     4,                              // width
 *     2,                              // height
 *     4 * 4,                          // stride
 *     lh_ui_pixmap_format_argb8888);  // format
 * @endcode
 *
 * @see lh_initializer
 * @see lh_ui_pixmap_t
 */
#define lh_ui_pixmap_initializer(bits, width, height, stride, format)                              \
    lh_initializer(bits, width, height, stride, format)

#endif /* LH_UI_PIXMAP_INITIALIZER_H */
