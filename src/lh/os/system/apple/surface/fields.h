/**
 * @file fields.h
 * @brief Member fields of ::lh_os_system_apple_surface_t.
 */

#ifndef LH_SRC_OS_SYSTEM_APPLE_SURFACE_FIELDS_H
#define LH_SRC_OS_SYSTEM_APPLE_SURFACE_FIELDS_H

/**
 * @def lh_os_system_apple_surface_fields
 * @brief The pixels and their shape.
 *
 * A 32-bit surface is top-down `0xAARRGGBB` words. A 16-bit surface is RGB565;
 * a present widens it into `staging`. `fit` is the letterboxed frame of the
 * last ::lh_os_system_surface_present_fit, reused when the destination size
 * does not change.
 */
#define lh_os_system_apple_surface_fields                                                          \
    lh_byte_ptr bits;                                                                              \
    int width;                                                                                     \
    int height;                                                                                    \
    int pixel_bits;                                                                                \
    lh_u32_t *staging;                                                                             \
    lh_usize_t staging_words;                                                                      \
    struct lh_os_system_apple_surface *fit

#endif /* LH_SRC_OS_SYSTEM_APPLE_SURFACE_FIELDS_H */
