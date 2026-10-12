/**
 * @file paint.h
 * @brief Backend-private: what the X window backend hands a paint as its "DC".
 *
 * Windows has a device context to paint into and a surface blits into it. X has
 * a window, a graphics context and a connection, and a present needs all three,
 * so this is what `paint_dc` points to for the length of a paint on Linux —
 * owned by the window, read by the surface.
 */

#ifndef LH_SRC_OS_SYSTEM_POSIX_PAINT_H
#define LH_SRC_OS_SYSTEM_POSIX_PAINT_H

#include <lh/numeric/fixed/types.h>
#include <lh/os/system/posix/x11.h>
#include <lh/os/system/posix/paint/fields.h>

/** @brief A paint in progress on one window. */
struct lh_os_system_posix_paint
{
    lh_os_system_posix_paint_fields;
};
typedef struct lh_os_system_posix_paint lh_os_system_posix_paint_t;

#endif /* LH_SRC_OS_SYSTEM_POSIX_PAINT_H */
