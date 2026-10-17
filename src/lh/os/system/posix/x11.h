/**
 * @file x11.h
 * @brief Backend-private: the part of libX11 the Linux window backend uses,
 *        declared by us instead of `<X11/Xlib.h>`.
 *
 * Same rules as ws2_32.h / user32.h on Windows: only what is called,
 * `lh`-prefixed types and constants, Xlib's own function and member names.
 * Xlib is an old ABI and its layouts are frozen, but they are **layouts**:
 * a structure Xlib writes into has to be the size Xlib thinks it is, field for
 * field. The previous version of this file declared `XEvent` as a type and five
 * small structs, and `XNextEvent` writes 192 bytes into whatever it is given —
 * so every event wrote past the end of the one it was read into. Every
 * structure here is therefore the whole of Xlib's, in Xlib's order, and the
 * event is padded to Xlib's 24 longs.
 *
 * libX11 is **not linked**: ::lh_os_system_posix_x11_load opens `libX11.so.6`
 * the first time a window is opened, and a program that never opens one never
 * needs it. The same library serves a Modbus poller on a headless server and a
 * configurator on a desktop; linking X11 in would make the first one fail to
 * start on the machine it is for.
 */

#ifndef LH_SRC_OS_SYSTEM_POSIX_X11_H
#define LH_SRC_OS_SYSTEM_POSIX_X11_H

#include <lh/numeric/types.h>
#include <lh/void.h>

/** @brief `XID`: every server resource (`Window`, `Atom`, `Pixmap`, ...). */
typedef unsigned long lh_os_system_posix_xid_t;

/** @brief `Display`: only ever a pointer. */
typedef struct lh_os_system_posix_display lh_os_system_posix_display_t;
/** @brief `Visual`: only ever a pointer. */
typedef struct lh_os_system_posix_visual lh_os_system_posix_visual_t;
/** @brief `GC`'s target: only ever a pointer. */
typedef struct lh_os_system_posix_gc lh_os_system_posix_gc_t;
/** @brief `XErrorEvent`: only ever a pointer, and never read. */
typedef struct lh_os_system_posix_xerror_event lh_os_system_posix_xerror_event_t;
/** @brief `XRenderPictFormat`: only ever a pointer. */
typedef struct lh_os_system_posix_xrender_format lh_os_system_posix_xrender_format_t;

#include <lh/os/system/posix/x11/xany_event/fields.h>
#include <lh/os/system/posix/x11/xbutton_event/fields.h>
#include <lh/os/system/posix/x11/xclient_message_event/fields.h>
#include <lh/os/system/posix/x11/xconfigure_event/fields.h>
#include <lh/os/system/posix/x11/xexpose_event/fields.h>
#include <lh/os/system/posix/x11/ximage/fields.h>
#include <lh/os/system/posix/x11/xkey_event/fields.h>
#include <lh/os/system/posix/x11/xmotion_event/fields.h>
#include <lh/os/system/posix/x11/xproperty_event/fields.h>
#include <lh/os/system/posix/x11/xrectangle/fields.h>
#include <lh/os/system/posix/x11/xrender_color/fields.h>
#include <lh/os/system/posix/x11/xset_window_attributes/fields.h>
#include <lh/os/system/posix/x11/xshm_segment_info/fields.h>
#include <lh/os/system/posix/x11/xsize_hints/fields.h>
#include <lh/os/system/posix/x11/xvisual_info/fields.h>

/** @brief `XAnyEvent`. */
struct lh_os_system_posix_xany_event
{
    /** Members: ::lh_os_system_posix_xany_event_fields. */
    lh_os_system_posix_xany_event_fields;
};
/** @copybrief lh_os_system_posix_xany_event */
typedef struct lh_os_system_posix_xany_event lh_os_system_posix_xany_event_t;

/** @brief `XKeyEvent`. */
struct lh_os_system_posix_xkey_event
{
    /** Members: ::lh_os_system_posix_xkey_event_fields. */
    lh_os_system_posix_xkey_event_fields;
};
/** @copybrief lh_os_system_posix_xkey_event */
typedef struct lh_os_system_posix_xkey_event lh_os_system_posix_xkey_event_t;

/** @brief `XButtonEvent`. */
struct lh_os_system_posix_xbutton_event
{
    /** Members: ::lh_os_system_posix_xbutton_event_fields. */
    lh_os_system_posix_xbutton_event_fields;
};
/** @copybrief lh_os_system_posix_xbutton_event */
typedef struct lh_os_system_posix_xbutton_event lh_os_system_posix_xbutton_event_t;

/** @brief `XMotionEvent`. */
struct lh_os_system_posix_xmotion_event
{
    /** Members: ::lh_os_system_posix_xmotion_event_fields. */
    lh_os_system_posix_xmotion_event_fields;
};
/** @copybrief lh_os_system_posix_xmotion_event */
typedef struct lh_os_system_posix_xmotion_event lh_os_system_posix_xmotion_event_t;

/** @brief `XExposeEvent`. */
struct lh_os_system_posix_xexpose_event
{
    /** Members: ::lh_os_system_posix_xexpose_event_fields. */
    lh_os_system_posix_xexpose_event_fields;
};
/** @copybrief lh_os_system_posix_xexpose_event */
typedef struct lh_os_system_posix_xexpose_event lh_os_system_posix_xexpose_event_t;

/** @brief `XConfigureEvent`. */
struct lh_os_system_posix_xconfigure_event
{
    /** Members: ::lh_os_system_posix_xconfigure_event_fields. */
    lh_os_system_posix_xconfigure_event_fields;
};
/** @copybrief lh_os_system_posix_xconfigure_event */
typedef struct lh_os_system_posix_xconfigure_event lh_os_system_posix_xconfigure_event_t;

/** @brief `XClientMessageEvent`. */
struct lh_os_system_posix_xclient_message_event
{
    /** Members: ::lh_os_system_posix_xclient_message_event_fields. */
    lh_os_system_posix_xclient_message_event_fields;
};
/** @copybrief lh_os_system_posix_xclient_message_event */
typedef struct lh_os_system_posix_xclient_message_event lh_os_system_posix_xclient_message_event_t;

/** @brief `XPropertyEvent`. */
struct lh_os_system_posix_xproperty_event
{
    /** Members: ::lh_os_system_posix_xproperty_event_fields. */
    lh_os_system_posix_xproperty_event_fields;
};
/** @copybrief lh_os_system_posix_xproperty_event */
typedef struct lh_os_system_posix_xproperty_event lh_os_system_posix_xproperty_event_t;

/**
 * @brief `XEvent`: every event Xlib can deliver, padded to its 24 longs.
 *
 * The pad is the point. `XNextEvent` copies a whole `XEvent` whatever the event
 * is, so this union has to be at least that big even though only the members
 * below are ever read.
 */
union lh_os_system_posix_xevent
{
    int type;                                          /**< which event; selects the member */
    lh_os_system_posix_xany_event_t any;               /**< what every event starts with */
    lh_os_system_posix_xkey_event_t key;               /**< `KeyPress` / `KeyRelease` */
    lh_os_system_posix_xbutton_event_t button;         /**< `ButtonPress` / `ButtonRelease` */
    lh_os_system_posix_xmotion_event_t motion;         /**< `MotionNotify` */
    lh_os_system_posix_xexpose_event_t expose;         /**< `Expose` */
    lh_os_system_posix_xconfigure_event_t configure;   /**< `ConfigureNotify` */
    lh_os_system_posix_xclient_message_event_t client; /**< `ClientMessage` */
    lh_os_system_posix_xproperty_event_t property;     /**< `PropertyNotify` */
    long pad[24];                                      /**< Xlib's full `XEvent` size */
};
/** @copybrief lh_os_system_posix_xevent */
typedef union lh_os_system_posix_xevent lh_os_system_posix_xevent_t;

/** @brief `XSetWindowAttributes`. */
struct lh_os_system_posix_xset_window_attributes
{
    /** Members: ::lh_os_system_posix_xset_window_attributes_fields. */
    lh_os_system_posix_xset_window_attributes_fields;
};
/** @copybrief lh_os_system_posix_xset_window_attributes */
typedef struct lh_os_system_posix_xset_window_attributes
    lh_os_system_posix_xset_window_attributes_t;

/** @brief `XSizeHints`. */
struct lh_os_system_posix_xsize_hints
{
    /** Members: ::lh_os_system_posix_xsize_hints_fields. */
    lh_os_system_posix_xsize_hints_fields;
};
/** @copybrief lh_os_system_posix_xsize_hints */
typedef struct lh_os_system_posix_xsize_hints lh_os_system_posix_xsize_hints_t;

/** @brief `XVisualInfo`. */
struct lh_os_system_posix_xvisual_info
{
    /** Members: ::lh_os_system_posix_xvisual_info_fields. */
    lh_os_system_posix_xvisual_info_fields;
};
/** @copybrief lh_os_system_posix_xvisual_info */
typedef struct lh_os_system_posix_xvisual_info lh_os_system_posix_xvisual_info_t;

/** @brief `XImage`. */
struct lh_os_system_posix_ximage
{
    /** Members: ::lh_os_system_posix_ximage_fields. */
    lh_os_system_posix_ximage_fields;
};
/** @copybrief lh_os_system_posix_ximage */
typedef struct lh_os_system_posix_ximage lh_os_system_posix_ximage_t;

/** @brief `XRectangle`. */
struct lh_os_system_posix_xrectangle
{
    /** Members: ::lh_os_system_posix_xrectangle_fields. */
    lh_os_system_posix_xrectangle_fields;
};
/** @copybrief lh_os_system_posix_xrectangle */
typedef struct lh_os_system_posix_xrectangle lh_os_system_posix_xrectangle_t;

/** @brief `XRenderColor`. */
struct lh_os_system_posix_xrender_color
{
    /** Members: ::lh_os_system_posix_xrender_color_fields. */
    lh_os_system_posix_xrender_color_fields;
};
/** @copybrief lh_os_system_posix_xrender_color */
typedef struct lh_os_system_posix_xrender_color lh_os_system_posix_xrender_color_t;

/** @brief `XShmSegmentInfo`. */
struct lh_os_system_posix_xshm_segment_info
{
    /** Members: ::lh_os_system_posix_xshm_segment_info_fields. */
    lh_os_system_posix_xshm_segment_info_fields;
};
/** @copybrief lh_os_system_posix_xshm_segment_info */
typedef struct lh_os_system_posix_xshm_segment_info lh_os_system_posix_xshm_segment_info_t;

/* -- Constants (X.h, Xutil.h, keysymdef.h) --------------------------------- */

/** `None`: no resource. */
#define LH_OS_SYSTEM_POSIX_X_NONE 0UL
/** `CurrentTime`: the server's time when the request is processed. */
#define LH_OS_SYSTEM_POSIX_X_CURRENT_TIME 0UL

/** @{ */
/** Event types. */
#define LH_OS_SYSTEM_POSIX_X_KEY_PRESS 2
#define LH_OS_SYSTEM_POSIX_X_KEY_RELEASE 3
#define LH_OS_SYSTEM_POSIX_X_BUTTON_PRESS 4
#define LH_OS_SYSTEM_POSIX_X_BUTTON_RELEASE 5
#define LH_OS_SYSTEM_POSIX_X_MOTION_NOTIFY 6
#define LH_OS_SYSTEM_POSIX_X_EXPOSE 12
#define LH_OS_SYSTEM_POSIX_X_MAP_NOTIFY 19
#define LH_OS_SYSTEM_POSIX_X_CONFIGURE_NOTIFY 22
#define LH_OS_SYSTEM_POSIX_X_PROPERTY_NOTIFY 28
#define LH_OS_SYSTEM_POSIX_X_CLIENT_MESSAGE 33
/** @} */

/** @{ */
/** Event masks. */
#define LH_OS_SYSTEM_POSIX_X_KEY_PRESS_MASK (1L << 0)
#define LH_OS_SYSTEM_POSIX_X_KEY_RELEASE_MASK (1L << 1)
#define LH_OS_SYSTEM_POSIX_X_BUTTON_PRESS_MASK (1L << 2)
#define LH_OS_SYSTEM_POSIX_X_BUTTON_RELEASE_MASK (1L << 3)
#define LH_OS_SYSTEM_POSIX_X_POINTER_MOTION_MASK (1L << 6)
#define LH_OS_SYSTEM_POSIX_X_EXPOSURE_MASK (1L << 15)
#define LH_OS_SYSTEM_POSIX_X_STRUCTURE_NOTIFY_MASK (1L << 17)
#define LH_OS_SYSTEM_POSIX_X_SUBSTRUCTURE_NOTIFY_MASK (1L << 19)
#define LH_OS_SYSTEM_POSIX_X_SUBSTRUCTURE_REDIRECT_MASK (1L << 20)
#define LH_OS_SYSTEM_POSIX_X_PROPERTY_CHANGE_MASK (1L << 22)
/** @} */

/** Modifier state. */
#define LH_OS_SYSTEM_POSIX_X_SHIFT_MASK (1U << 0)

/** @{ */
/** XCreateWindow: class, and the value-mask bits for the attributes set. */
#define LH_OS_SYSTEM_POSIX_X_INPUT_OUTPUT 1U
#define LH_OS_SYSTEM_POSIX_X_COPY_FROM_PARENT 0
#define LH_OS_SYSTEM_POSIX_X_CW_BACK_PIXMAP (1UL << 0)
#define LH_OS_SYSTEM_POSIX_X_CW_BACK_PIXEL (1UL << 1)
#define LH_OS_SYSTEM_POSIX_X_CW_BORDER_PIXEL (1UL << 3)
#define LH_OS_SYSTEM_POSIX_X_CW_BIT_GRAVITY (1UL << 4)
#define LH_OS_SYSTEM_POSIX_X_CW_EVENT_MASK (1UL << 11)
#define LH_OS_SYSTEM_POSIX_X_CW_COLORMAP (1UL << 13)
#define LH_OS_SYSTEM_POSIX_X_TRUE_COLOR 4
#define LH_OS_SYSTEM_POSIX_X_ALLOC_NONE 0
#define LH_OS_SYSTEM_POSIX_X_NORTH_WEST_GRAVITY 1
/** @} */

/** @{ */
/** Image formats and byte orders. */
#define LH_OS_SYSTEM_POSIX_X_ZPIXMAP 2
#define LH_OS_SYSTEM_POSIX_X_LSB_FIRST 0
#define LH_OS_SYSTEM_POSIX_X_MSB_FIRST 1
/** @} */

/** @{ */
/** Properties. */
#define LH_OS_SYSTEM_POSIX_X_PROP_MODE_REPLACE 0
#define LH_OS_SYSTEM_POSIX_X_SUCCESS 0
#define LH_OS_SYSTEM_POSIX_X_XA_ATOM 4UL
#define LH_OS_SYSTEM_POSIX_X_XA_CARDINAL 6UL
/** @} */

/** XEventsQueued modes. */
#define LH_OS_SYSTEM_POSIX_X_QUEUED_AFTER_READING 1

/** @{ */
/** XSizeHints flags. */
#define LH_OS_SYSTEM_POSIX_X_US_POSITION (1L << 0)
#define LH_OS_SYSTEM_POSIX_X_P_POSITION (1L << 2)
/** @} */

/** @{ */
/** Pointer buttons. 4 and 5 are the vertical wheel, 6 and 7 the horizontal one. */
#define LH_OS_SYSTEM_POSIX_X_BUTTON_LEFT 1U
#define LH_OS_SYSTEM_POSIX_X_BUTTON_WHEEL_UP 4U
#define LH_OS_SYSTEM_POSIX_X_BUTTON_WHEEL_DOWN 5U
#define LH_OS_SYSTEM_POSIX_X_BUTTON_WHEEL_LEFT 6U
#define LH_OS_SYSTEM_POSIX_X_BUTTON_WHEEL_RIGHT 7U
/** @} */

/** @{ */
/** SHAPE (shape.h): the bounding shape, replaced, rectangles in no promised order. */
#define LH_OS_SYSTEM_POSIX_X_SHAPE_BOUNDING 0
#define LH_OS_SYSTEM_POSIX_X_SHAPE_SET 0
#define LH_OS_SYSTEM_POSIX_X_SHAPE_UNSORTED 0
/** @} */

/** @{ */
/** RENDER (render.h): replace the destination; the standard xRGB format. */
#define LH_OS_SYSTEM_POSIX_X_PICT_OP_SRC 1
#define LH_OS_SYSTEM_POSIX_X_PICT_STANDARD_RGB24 1
/** @} */

/* -- The library ----------------------------------------------------------- */

#include <lh/os/system/posix/x11/fields.h>

/**
 * @brief libX11, opened at run time: one function pointer per call the backend
 *        makes, named as Xlib names them.
 */
struct lh_os_system_posix_x11
{
    /** Members: ::lh_os_system_posix_x11_fields. */
    lh_os_system_posix_x11_fields;
};
/** @copybrief lh_os_system_posix_x11 */
typedef struct lh_os_system_posix_x11 lh_os_system_posix_x11_t;

/**
 * @brief libX11, loaded on the first call and kept for the life of the process.
 *
 * @return The functions, or `lh_null` when there is no libX11 on this machine or
 *         it lacks one of them — a machine without X has no windows, which is a
 *         window that does not open, not a program that does not start.
 */
const lh_os_system_posix_x11_t *
lh_os_system_posix_x11_load(void);

#include <lh/os/system/posix/x11/xext/fields.h>

/**
 * @brief libXext, opened at run time: SHAPE and MIT-SHM.
 */
struct lh_os_system_posix_xext
{
    /** Members: ::lh_os_system_posix_xext_fields. */
    lh_os_system_posix_xext_fields;
};
/** @copybrief lh_os_system_posix_xext */
typedef struct lh_os_system_posix_xext lh_os_system_posix_xext_t;

/**
 * @brief libXext, loaded on the first call and kept.
 *
 * @return The functions, or `lh_null` when there is no libXext. Both extensions
 *         are things a window is better with and works without: square corners,
 *         and pixels sent through the socket.
 */
const lh_os_system_posix_xext_t *
lh_os_system_posix_xext_load(void);

#include <lh/os/system/posix/x11/xrender/fields.h>

/**
 * @brief libXrender, opened at run time.
 */
struct lh_os_system_posix_xrender
{
    /** Members: ::lh_os_system_posix_xrender_fields. */
    lh_os_system_posix_xrender_fields;
};
/** @copybrief lh_os_system_posix_xrender */
typedef struct lh_os_system_posix_xrender lh_os_system_posix_xrender_t;

/**
 * @brief libXrender, loaded on the first call and kept.
 *
 * @return The functions, or `lh_null` when there is no libXrender: a window then
 *         has no alpha of its own, and round corners are cut by SHAPE alone.
 */
const lh_os_system_posix_xrender_t *
lh_os_system_posix_xrender_load(void);

#endif /* LH_SRC_OS_SYSTEM_POSIX_X11_H */
