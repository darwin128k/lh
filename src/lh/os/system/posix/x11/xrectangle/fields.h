/**
 * @file fields.h
 * @brief Member fields of ::lh_os_system_posix_xrectangle_t.
 */

#ifndef LH_SRC_OS_SYSTEM_POSIX_X11_XRECTANGLE_FIELDS_H
#define LH_SRC_OS_SYSTEM_POSIX_X11_XRECTANGLE_FIELDS_H

/**
 * @def lh_os_system_posix_xrectangle_fields
 * @brief `XRectangle`: one rectangle of a shape, in 16-bit coordinates.
 */
#define lh_os_system_posix_xrectangle_fields \
    short x;                                                                                       \
    short y;                                                                                       \
    unsigned short width;                                                                          \
    unsigned short height

#endif /* LH_SRC_OS_SYSTEM_POSIX_X11_XRECTANGLE_FIELDS_H */
