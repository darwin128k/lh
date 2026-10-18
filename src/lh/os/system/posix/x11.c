/**
 * @file x11.c
 * @brief libX11, opened when the first window is.
 */

#include <lh/os/system/posix/x11.h>

#include <lh/null.h>
#include <lh/util/addr.h>

#include <dlfcn.h>

/** libX11's entry points, once loaded. */
static lh_os_system_posix_x11_t lh_os_system_posix_x11;

/** 0 not tried yet, 1 loaded, -1 tried and there is none. Tried once: a machine
   that has no libX11 will not grow one, and dlopen is not free. */
static int lh_os_system_posix_x11_state;

/** libXext's entry points, once loaded. */
static lh_os_system_posix_xext_t lh_os_system_posix_xext;
/** Same states as ::lh_os_system_posix_x11_state, for libXext. */
static int lh_os_system_posix_xext_state;

/** libXrender's entry points, once loaded. */
static lh_os_system_posix_xrender_t lh_os_system_posix_xrender;
/** Same states as ::lh_os_system_posix_x11_state, for libXrender. */
static int lh_os_system_posix_xrender_state;

/** @{ */
/** Each pointer is filled from the symbol of its own name. Assigned through a
   `void **` because ISO C has no conversion from `void *` to a function pointer,
   and dlsym returns the one to stand for the other. */
#define LH_OS_SYSTEM_POSIX_X11_LOAD_INTO(table, name)                                              \
    do                                                                                             \
    {                                                                                              \
        void *symbol = dlsym(library, #name);                                                      \
        if (lh_null_eq(symbol))                                                                    \
        {                                                                                          \
            return lh_null;                                                                        \
        }                                                                                          \
        *(void **)(lh_addr_of((table).name)) = symbol;                                             \
    } while (0)
#define LH_OS_SYSTEM_POSIX_X11_LOAD(name)                                                          \
    LH_OS_SYSTEM_POSIX_X11_LOAD_INTO(lh_os_system_posix_x11, name)
#define LH_OS_SYSTEM_POSIX_XEXT_LOAD(name)                                                         \
    LH_OS_SYSTEM_POSIX_X11_LOAD_INTO(lh_os_system_posix_xext, name)
#define LH_OS_SYSTEM_POSIX_XRENDER_LOAD(name)                                                      \
    LH_OS_SYSTEM_POSIX_X11_LOAD_INTO(lh_os_system_posix_xrender, name)
/** @} */

/** Load libX11 and every symbol the backend calls; ::lh_null when it is not there. */
static const lh_os_system_posix_x11_t *
lh_os_system_posix_x11_open(void)
{
    /* The versioned name is the one every distribution installs with the runtime;
       the bare one exists only with the development package. */
    void *library = dlopen("libX11.so.6", RTLD_NOW | RTLD_LOCAL);

    if (lh_null_eq(library))
    {
        library = dlopen("libX11.so", RTLD_NOW | RTLD_LOCAL);
    }
    if (lh_null_eq(library))
    {
        return lh_null;
    }
    LH_OS_SYSTEM_POSIX_X11_LOAD(XOpenDisplay);
    LH_OS_SYSTEM_POSIX_X11_LOAD(XCloseDisplay);
    LH_OS_SYSTEM_POSIX_X11_LOAD(XConnectionNumber);
    LH_OS_SYSTEM_POSIX_X11_LOAD(XDefaultScreen);
    LH_OS_SYSTEM_POSIX_X11_LOAD(XRootWindow);
    LH_OS_SYSTEM_POSIX_X11_LOAD(XDefaultVisual);
    LH_OS_SYSTEM_POSIX_X11_LOAD(XDefaultDepth);
    LH_OS_SYSTEM_POSIX_X11_LOAD(XDisplayWidth);
    LH_OS_SYSTEM_POSIX_X11_LOAD(XDisplayHeight);
    LH_OS_SYSTEM_POSIX_X11_LOAD(XCreateWindow);
    LH_OS_SYSTEM_POSIX_X11_LOAD(XDestroyWindow);
    LH_OS_SYSTEM_POSIX_X11_LOAD(XMapWindow);
    LH_OS_SYSTEM_POSIX_X11_LOAD(XStoreName);
    LH_OS_SYSTEM_POSIX_X11_LOAD(XChangeProperty);
    LH_OS_SYSTEM_POSIX_X11_LOAD(XGetWindowProperty);
    LH_OS_SYSTEM_POSIX_X11_LOAD(XFree);
    LH_OS_SYSTEM_POSIX_X11_LOAD(XInternAtom);
    LH_OS_SYSTEM_POSIX_X11_LOAD(XSetWMProtocols);
    LH_OS_SYSTEM_POSIX_X11_LOAD(XSetWMNormalHints);
    LH_OS_SYSTEM_POSIX_X11_LOAD(XSetTransientForHint);
    LH_OS_SYSTEM_POSIX_X11_LOAD(XCreateGC);
    LH_OS_SYSTEM_POSIX_X11_LOAD(XFreeGC);
    LH_OS_SYSTEM_POSIX_X11_LOAD(XInitImage);
    LH_OS_SYSTEM_POSIX_X11_LOAD(XPutImage);
    LH_OS_SYSTEM_POSIX_X11_LOAD(XPending);
    LH_OS_SYSTEM_POSIX_X11_LOAD(XEventsQueued);
    LH_OS_SYSTEM_POSIX_X11_LOAD(XNextEvent);
    LH_OS_SYSTEM_POSIX_X11_LOAD(XPeekEvent);
    LH_OS_SYSTEM_POSIX_X11_LOAD(XCheckTypedWindowEvent);
    LH_OS_SYSTEM_POSIX_X11_LOAD(XSendEvent);
    LH_OS_SYSTEM_POSIX_X11_LOAD(XFlush);
    LH_OS_SYSTEM_POSIX_X11_LOAD(XSync);
    LH_OS_SYSTEM_POSIX_X11_LOAD(XLookupString);
    LH_OS_SYSTEM_POSIX_X11_LOAD(XTranslateCoordinates);
    LH_OS_SYSTEM_POSIX_X11_LOAD(XGetGeometry);
    LH_OS_SYSTEM_POSIX_X11_LOAD(XMatchVisualInfo);
    LH_OS_SYSTEM_POSIX_X11_LOAD(XCreateColormap);
    LH_OS_SYSTEM_POSIX_X11_LOAD(XFreeColormap);
    LH_OS_SYSTEM_POSIX_X11_LOAD(XCreatePixmap);
    LH_OS_SYSTEM_POSIX_X11_LOAD(XFreePixmap);
    LH_OS_SYSTEM_POSIX_X11_LOAD(XUngrabPointer);
    LH_OS_SYSTEM_POSIX_X11_LOAD(XIconifyWindow);
    LH_OS_SYSTEM_POSIX_X11_LOAD(XSetErrorHandler);
    /* The library stays open: every pointer above points into it. */
    return lh_addr_of(lh_os_system_posix_x11);
}

const lh_os_system_posix_x11_t *
lh_os_system_posix_x11_load(void)
{
    if (lh_os_system_posix_x11_state == 0)
    {
        lh_os_system_posix_x11_state = lh_null_ne(lh_os_system_posix_x11_open()) ? 1 : -1;
    }
    return lh_os_system_posix_x11_state > 0 ? lh_addr_of(lh_os_system_posix_x11) : lh_null;
}

/** Load libXext (MIT-SHM, SHAPE); ::lh_null when it is not there. */
static const lh_os_system_posix_xext_t *
lh_os_system_posix_xext_open(void)
{
    void *library = dlopen("libXext.so.6", RTLD_NOW | RTLD_LOCAL);

    if (lh_null_eq(library))
    {
        library = dlopen("libXext.so", RTLD_NOW | RTLD_LOCAL);
    }
    if (lh_null_eq(library))
    {
        return lh_null;
    }
    LH_OS_SYSTEM_POSIX_XEXT_LOAD(XShapeQueryExtension);
    LH_OS_SYSTEM_POSIX_XEXT_LOAD(XShapeCombineRectangles);
    LH_OS_SYSTEM_POSIX_XEXT_LOAD(XShapeCombineMask);
    LH_OS_SYSTEM_POSIX_XEXT_LOAD(XShmQueryExtension);
    LH_OS_SYSTEM_POSIX_XEXT_LOAD(XShmAttach);
    LH_OS_SYSTEM_POSIX_XEXT_LOAD(XShmDetach);
    LH_OS_SYSTEM_POSIX_XEXT_LOAD(XShmPutImage);
    return lh_addr_of(lh_os_system_posix_xext);
}

const lh_os_system_posix_xext_t *
lh_os_system_posix_xext_load(void)
{
    if (lh_os_system_posix_xext_state == 0)
    {
        lh_os_system_posix_xext_state = lh_null_ne(lh_os_system_posix_xext_open()) ? 1 : -1;
    }
    return lh_os_system_posix_xext_state > 0 ? lh_addr_of(lh_os_system_posix_xext) : lh_null;
}

/** Load libXrender; ::lh_null when it is not there. */
static const lh_os_system_posix_xrender_t *
lh_os_system_posix_xrender_open(void)
{
    void *library = dlopen("libXrender.so.1", RTLD_NOW | RTLD_LOCAL);

    if (lh_null_eq(library))
    {
        library = dlopen("libXrender.so", RTLD_NOW | RTLD_LOCAL);
    }
    if (lh_null_eq(library))
    {
        return lh_null;
    }
    LH_OS_SYSTEM_POSIX_XRENDER_LOAD(XRenderQueryExtension);
    LH_OS_SYSTEM_POSIX_XRENDER_LOAD(XRenderFindVisualFormat);
    LH_OS_SYSTEM_POSIX_XRENDER_LOAD(XRenderFindStandardFormat);
    LH_OS_SYSTEM_POSIX_XRENDER_LOAD(XRenderCreatePicture);
    LH_OS_SYSTEM_POSIX_XRENDER_LOAD(XRenderFreePicture);
    LH_OS_SYSTEM_POSIX_XRENDER_LOAD(XRenderComposite);
    LH_OS_SYSTEM_POSIX_XRENDER_LOAD(XRenderFillRectangles);
    return lh_addr_of(lh_os_system_posix_xrender);
}

const lh_os_system_posix_xrender_t *
lh_os_system_posix_xrender_load(void)
{
    if (lh_os_system_posix_xrender_state == 0)
    {
        lh_os_system_posix_xrender_state = lh_null_ne(lh_os_system_posix_xrender_open()) ? 1 : -1;
    }
    return lh_os_system_posix_xrender_state > 0 ? lh_addr_of(lh_os_system_posix_xrender) : lh_null;
}
