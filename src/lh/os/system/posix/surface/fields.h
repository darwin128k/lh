/**
 * @file fields.h
 * @brief Member fields of ::lh_os_system_posix_surface_t.
 */

#ifndef LH_SRC_OS_SYSTEM_POSIX_SURFACE_FIELDS_H
#define LH_SRC_OS_SYSTEM_POSIX_SURFACE_FIELDS_H

/**
 * @def lh_os_system_posix_surface_fields
 * @brief The pixels, their shape, and a 32-bit staging buffer for a 16-bit
 *        surface (X windows are 24 or 32 bits deep, and nothing converts on the way).
 *
 * A 32-bit surface lives in a shared-memory `segment` when the server can read it
 * from there (`shared`); `pending` is a present the server may not have read yet,
 * which the next write has to wait for.
 */
#define lh_os_system_posix_surface_fields                                                          \
    lh_byte_ptr bits;                                                                              \
    int width;                                                                                     \
    int height;                                                                                    \
    int pixel_bits;                                                                                \
    lh_u32_t *staging;                                                                             \
    lh_usize_t staging_words;                                                                      \
    lh_os_system_posix_xshm_segment_info_t segment;                                                \
    lh_os_system_posix_display_t *display;                                                         \
    lh_bool_t shared;                                                                              \
    lh_bool_t pending

#endif /* LH_SRC_OS_SYSTEM_POSIX_SURFACE_FIELDS_H */
