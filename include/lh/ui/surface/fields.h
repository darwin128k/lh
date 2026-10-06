/**
 * @file fields.h
 * @brief Member fields of ::lh_ui_surface_t.
 */

#ifndef LH_UI_SURFACE_FIELDS_H
#define LH_UI_SURFACE_FIELDS_H

/**
 * @def lh_ui_surface_fields(size_type, handle_type)
 * @brief Pixel size and the OS surface handle (not a second DC bolted on).
 *
 * @param size_type   ::lh_ui_size_t.
 * @param handle_type ::lh_os_system_surface_handle_t.
 */
#define lh_ui_surface_fields(size_type, handle_type)                                                \
    size_type size;                                                                                 \
    handle_type handle

#endif /* LH_UI_SURFACE_FIELDS_H */
