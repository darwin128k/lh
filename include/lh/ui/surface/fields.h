/**
 * @file fields.h
 * @brief Member fields of ::lh_ui_surface_t.
 */

#ifndef LH_UI_SURFACE_FIELDS_H
#define LH_UI_SURFACE_FIELDS_H

/**
 * @def lh_ui_surface_fields(size_type, format_type, handle_type)
 * @brief Pixel size, pixel format, and the OS surface handle (not a second DC
 *        bolted on).
 *
 * @param size_type   ::lh_ui_size_t.
 * @param format_type ::lh_ui_pixmap_format_t.
 * @param handle_type ::lh_os_system_surface_handle_t.
 *
 * Size and format are one property of the same buffer — asking for the pixels of
 * one size in another format is the same buffer twice over — so they are asked for
 * together and kept in that order.
 */
#define lh_ui_surface_fields(size_type, format_type, handle_type)                                  \
    size_type size;                                                                                 \
    format_type format;                                                                             \
    handle_type handle

#endif /* LH_UI_SURFACE_FIELDS_H */
