/**
 * @file fields.h
 * @brief Member fields of ::lh_os_system_apple_paint_t.
 */

#ifndef LH_SRC_OS_SYSTEM_APPLE_PAINT_FIELDS_H
#define LH_SRC_OS_SYSTEM_APPLE_PAINT_FIELDS_H

/**
 * @def lh_os_system_apple_paint_fields
 * @brief The graphics context a present draws into, and the area of this paint.
 *
 * The context is an AppKit `CGContextRef` stored as a pointer so this header
 * stays C. It is valid only for the paint that created it.
 */
#define lh_os_system_apple_paint_fields                                                            \
    lh_ptr context;                                                                                \
    lh_u64_t area;                                                                                 \
    lh_u32_t rects

#endif /* LH_SRC_OS_SYSTEM_APPLE_PAINT_FIELDS_H */
