/**
 * @file fields.h
 * @brief Member fields of ::lh_os_system_posix_xrender_t.
 */

#ifndef LH_SRC_OS_SYSTEM_POSIX_X11_XRENDER_FIELDS_H
#define LH_SRC_OS_SYSTEM_POSIX_X11_XRENDER_FIELDS_H

/**
 * @def lh_os_system_posix_xrender_fields
 * @brief The libXrender calls this backend makes: enough to copy a 24-bit picture
 *        into a 32-bit window -- which is where the window's alpha comes from --
 *        and to clear the corners of one to transparent.
 *
 * `Picture` is an ::lh_os_system_posix_xid_t; `XRenderPictFormat` is only ever a
 * pointer.
 */
#define lh_os_system_posix_xrender_fields \
    int (*XRenderQueryExtension)(lh_os_system_posix_display_t *display, int *event_base,           \
                                 int *error_base);                                                 \
    lh_os_system_posix_xrender_format_t *(*XRenderFindVisualFormat)(lh_os_system_posix_display_t *display,\
                                                                   const lh_os_system_posix_visual_t *visual);\
    lh_os_system_posix_xrender_format_t *(*XRenderFindStandardFormat)(lh_os_system_posix_display_t *display,\
                                                                     int format);                  \
    lh_os_system_posix_xid_t (*XRenderCreatePicture)(lh_os_system_posix_display_t *display,        \
                                                     lh_os_system_posix_xid_t drawable,            \
                                                     const lh_os_system_posix_xrender_format_t *format,\
                                                     unsigned long mask, const void *attributes);  \
    lh_void (*XRenderFreePicture)(lh_os_system_posix_display_t *display, lh_os_system_posix_xid_t picture);\
    lh_void (*XRenderComposite)(lh_os_system_posix_display_t *display, int op, lh_os_system_posix_xid_t source,\
                             lh_os_system_posix_xid_t mask, lh_os_system_posix_xid_t destination,  \
                             int source_x, int source_y, int mask_x, int mask_y, int x, int y,     \
                             unsigned int width, unsigned int height);                             \
    lh_void (*XRenderFillRectangles)(lh_os_system_posix_display_t *display, int op,                   \
                                  lh_os_system_posix_xid_t destination,                            \
                                  const lh_os_system_posix_xrender_color_t *color,                 \
                                  const lh_os_system_posix_xrectangle_t *rectangles, int count)

#endif /* LH_SRC_OS_SYSTEM_POSIX_X11_XRENDER_FIELDS_H */
