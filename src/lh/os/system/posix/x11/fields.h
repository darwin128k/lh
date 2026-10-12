/**
 * @file fields.h
 * @brief Member fields of ::lh_os_system_posix_x11_t.
 */

#ifndef LH_SRC_OS_SYSTEM_POSIX_X11_FIELDS_H
#define LH_SRC_OS_SYSTEM_POSIX_X11_FIELDS_H

/**
 * @def lh_os_system_posix_x11_fields
 * @brief One pointer per libX11 call, with Xlib's name and Xlib's signature.
 *
 * `Window`, `Atom`, `Drawable` and `Time` are all ::lh_os_system_posix_xid_t
 * (an `unsigned long`); `Bool` and `Status` are `int`; `GC` is a pointer.
 */
#define lh_os_system_posix_x11_fields                                                              \
    lh_os_system_posix_display_t *(*XOpenDisplay)(const char *name);                               \
    int (*XCloseDisplay)(lh_os_system_posix_display_t *display);                                   \
    int (*XConnectionNumber)(lh_os_system_posix_display_t *display);                               \
    int (*XDefaultScreen)(lh_os_system_posix_display_t *display);                                  \
    lh_os_system_posix_xid_t (*XRootWindow)(lh_os_system_posix_display_t *display, int screen);    \
    lh_os_system_posix_visual_t *(*XDefaultVisual)(lh_os_system_posix_display_t *display, int screen); \
    int (*XDefaultDepth)(lh_os_system_posix_display_t *display, int screen);                       \
    int (*XDisplayWidth)(lh_os_system_posix_display_t *display, int screen);                       \
    int (*XDisplayHeight)(lh_os_system_posix_display_t *display, int screen);                      \
    lh_os_system_posix_xid_t (*XCreateWindow)(                                                     \
        lh_os_system_posix_display_t *display, lh_os_system_posix_xid_t parent, int x, int y,      \
        unsigned int width, unsigned int height, unsigned int border, int depth, unsigned int klass, \
        lh_os_system_posix_visual_t *visual, unsigned long value_mask,                             \
        lh_os_system_posix_xset_window_attributes_t *attributes);                                  \
    int (*XDestroyWindow)(lh_os_system_posix_display_t *display, lh_os_system_posix_xid_t window); \
    int (*XMapWindow)(lh_os_system_posix_display_t *display, lh_os_system_posix_xid_t window);     \
    int (*XStoreName)(lh_os_system_posix_display_t *display, lh_os_system_posix_xid_t window,      \
                      const char *name);                                                           \
    int (*XChangeProperty)(lh_os_system_posix_display_t *display, lh_os_system_posix_xid_t window, \
                           lh_os_system_posix_xid_t property, lh_os_system_posix_xid_t type,       \
                           int format, int mode, const unsigned char *data, int count);            \
    int (*XGetWindowProperty)(lh_os_system_posix_display_t *display, lh_os_system_posix_xid_t window, \
                              lh_os_system_posix_xid_t property, long offset, long length,         \
                              int remove, lh_os_system_posix_xid_t type,                           \
                              lh_os_system_posix_xid_t *actual_type, int *actual_format,           \
                              unsigned long *count, unsigned long *bytes_after,                    \
                              unsigned char **data);                                               \
    int (*XFree)(void *data);                                                                      \
    lh_os_system_posix_xid_t (*XInternAtom)(lh_os_system_posix_display_t *display,                 \
                                            const char *name, int only_if_exists);                 \
    int (*XSetWMProtocols)(lh_os_system_posix_display_t *display, lh_os_system_posix_xid_t window, \
                           lh_os_system_posix_xid_t *protocols, int count);                        \
    lh_void (*XSetWMNormalHints)(lh_os_system_posix_display_t *display,                               \
                              lh_os_system_posix_xid_t window,                                     \
                              lh_os_system_posix_xsize_hints_t *hints);                            \
    int (*XSetTransientForHint)(lh_os_system_posix_display_t *display,                             \
                                lh_os_system_posix_xid_t window, lh_os_system_posix_xid_t owner);  \
    lh_os_system_posix_gc_t *(*XCreateGC)(lh_os_system_posix_display_t *display,                   \
                                          lh_os_system_posix_xid_t drawable, unsigned long mask,   \
                                          void *values);                                           \
    int (*XFreeGC)(lh_os_system_posix_display_t *display, lh_os_system_posix_gc_t *gc);            \
    int (*XInitImage)(lh_os_system_posix_ximage_t *image);                                         \
    int (*XPutImage)(lh_os_system_posix_display_t *display, lh_os_system_posix_xid_t drawable,     \
                     lh_os_system_posix_gc_t *gc, lh_os_system_posix_ximage_t *image, int src_x,   \
                     int src_y, int dest_x, int dest_y, unsigned int width, unsigned int height);  \
    int (*XPending)(lh_os_system_posix_display_t *display);                                        \
    int (*XEventsQueued)(lh_os_system_posix_display_t *display, int mode);                         \
    int (*XNextEvent)(lh_os_system_posix_display_t *display, lh_os_system_posix_xevent_t *event);  \
    int (*XPeekEvent)(lh_os_system_posix_display_t *display, lh_os_system_posix_xevent_t *event);  \
    int (*XCheckTypedWindowEvent)(lh_os_system_posix_display_t *display,                           \
                                  lh_os_system_posix_xid_t window, int type,                       \
                                  lh_os_system_posix_xevent_t *event);                             \
    int (*XSendEvent)(lh_os_system_posix_display_t *display, lh_os_system_posix_xid_t window,      \
                      int propagate, long mask, lh_os_system_posix_xevent_t *event);               \
    int (*XFlush)(lh_os_system_posix_display_t *display);                                          \
    int (*XSync)(lh_os_system_posix_display_t *display, int discard);                              \
    int (*XLookupString)(lh_os_system_posix_xkey_event_t *event, char *buffer, int size,           \
                         lh_os_system_posix_xid_t *keysym, void *status);                          \
    int (*XTranslateCoordinates)(lh_os_system_posix_display_t *display,                            \
                                 lh_os_system_posix_xid_t from, lh_os_system_posix_xid_t to,       \
                                 int from_x, int from_y, int *to_x, int *to_y,                     \
                                 lh_os_system_posix_xid_t *child);                                 \
    int (*XGetGeometry)(lh_os_system_posix_display_t *display, lh_os_system_posix_xid_t drawable,  \
                        lh_os_system_posix_xid_t *root, int *x, int *y, unsigned int *width,       \
                        unsigned int *height, unsigned int *border, unsigned int *depth);          \
    int (*XMatchVisualInfo)(lh_os_system_posix_display_t *display, int screen, int depth, int klass, \
                            lh_os_system_posix_xvisual_info_t *info);                              \
    lh_os_system_posix_xid_t (*XCreateColormap)(lh_os_system_posix_display_t *display,             \
                                                lh_os_system_posix_xid_t window,                   \
                                                lh_os_system_posix_visual_t *visual, int alloc);   \
    int (*XFreeColormap)(lh_os_system_posix_display_t *display, lh_os_system_posix_xid_t colormap);\
    lh_os_system_posix_xid_t (*XCreatePixmap)(lh_os_system_posix_display_t *display,               \
                                              lh_os_system_posix_xid_t drawable, unsigned int width,\
                                              unsigned int height, unsigned int depth);            \
    int (*XFreePixmap)(lh_os_system_posix_display_t *display, lh_os_system_posix_xid_t pixmap);    \
    int (*XUngrabPointer)(lh_os_system_posix_display_t *display, lh_os_system_posix_xid_t time);   \
    int (*XIconifyWindow)(lh_os_system_posix_display_t *display, lh_os_system_posix_xid_t window,  \
                          int screen);                                                             \
    int (*(*XSetErrorHandler)(int (*handler)(lh_os_system_posix_display_t *,                       \
                                             lh_os_system_posix_xerror_event_t *)))(               \
        lh_os_system_posix_display_t *, lh_os_system_posix_xerror_event_t *)

#endif /* LH_SRC_OS_SYSTEM_POSIX_X11_FIELDS_H */
