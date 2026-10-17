/**
 * @file fields.h
 * @brief Member fields of ::lh_os_system_posix_xext_t.
 */

#ifndef LH_SRC_OS_SYSTEM_POSIX_X11_XEXT_FIELDS_H
#define LH_SRC_OS_SYSTEM_POSIX_X11_XEXT_FIELDS_H

/**
 * @def lh_os_system_posix_xext_fields
 * @brief The libXext calls this backend makes: SHAPE, to cut a window's corners,
 *        and MIT-SHM, to present without copying the pixels through the socket.
 */
#define lh_os_system_posix_xext_fields                                                             \
    int (*XShapeQueryExtension)(lh_os_system_posix_display_t * display, int *event_base,           \
                                int *error_base);                                                  \
    lh_void (*XShapeCombineRectangles)(lh_os_system_posix_display_t * display,                     \
                                       lh_os_system_posix_xid_t window, int kind, int x, int y,    \
                                       lh_os_system_posix_xrectangle_t *rectangles, int count,     \
                                       int operation, int ordering);                               \
    lh_void (*XShapeCombineMask)(lh_os_system_posix_display_t * display,                           \
                                 lh_os_system_posix_xid_t window, int kind, int x, int y,          \
                                 lh_os_system_posix_xid_t mask, int operation);                    \
    int (*XShmQueryExtension)(lh_os_system_posix_display_t * display);                             \
    int (*XShmAttach)(lh_os_system_posix_display_t * display,                                      \
                      lh_os_system_posix_xshm_segment_info_t * segment);                           \
    int (*XShmDetach)(lh_os_system_posix_display_t * display,                                      \
                      lh_os_system_posix_xshm_segment_info_t * segment);                           \
    int (*XShmPutImage)(lh_os_system_posix_display_t * display, lh_os_system_posix_xid_t drawable, \
                        lh_os_system_posix_gc_t * gc, lh_os_system_posix_ximage_t * image,         \
                        int src_x, int src_y, int dest_x, int dest_y, unsigned int width,          \
                        unsigned int height, int send_event)

#endif /* LH_SRC_OS_SYSTEM_POSIX_X11_XEXT_FIELDS_H */
