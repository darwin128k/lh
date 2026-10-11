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
 */
#define lh_os_system_posix_surface_fields                                                          \
    lh_byte_t *bits;                                                                               \
    int width;                                                                                     \
    int height;                                                                                    \
    int pixel_bits;                                                                                \
    lh_u32_t *staging;                                                                             \
    lh_usize_t staging_words

#endif /* LH_SRC_OS_SYSTEM_POSIX_SURFACE_FIELDS_H */
