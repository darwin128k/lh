/**
 * @file corner.h
 * @brief Backend-private: XRectangle rows for a round-rect window cut.
 *
 * Pure fills of ::lh_os_system_posix_xrectangle_t from
 * ::lh_os_window_corner_inset — no Display. Testable without a server.
 */

#ifndef LH_SRC_OS_SYSTEM_POSIX_WINDOW_CORNER_H
#define LH_SRC_OS_SYSTEM_POSIX_WINDOW_CORNER_H

#include <lh/os/system/posix/x11.h>
#include <lh/os/window/corner.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Rows of a round-rect of @p radius over @p width x @p height.
 *
 * One rectangle per row of the top corners and the bottom ones, and one for
 * everything between. @p rows must hold `2 * radius + 1` entries (at most
 * `2 * ::LH_OS_WINDOW_CORNER_MAX + 1`).
 *
 * @return How many rectangles were written.
 */
int
lh_os_system_posix_corner_rows(lh_os_system_posix_xrectangle_t *rows, int width, int height,
                               int radius);

/**
 * @brief Rows of the corners outside a round-rect of @p radius (left and right
 *        of every cut row).
 *
 * @p rows must hold `4 * radius` entries (at most `4 * ::LH_OS_WINDOW_CORNER_MAX`).
 *
 * @return How many rectangles were written.
 */
int
lh_os_system_posix_corner_outside(lh_os_system_posix_xrectangle_t *rows, int width, int height,
                                  int radius);

LH_COMPILER_EXTERN_C_END

#endif /* LH_SRC_OS_SYSTEM_POSIX_WINDOW_CORNER_H */
