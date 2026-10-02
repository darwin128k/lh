/**
 * @file geom.h
 * @brief Backend-private: lh::geom ↔ Win32 native conversions.
 *
 * Lives under `src/lh/os/system/win/` (not installed, never installed).
 * Used by `src/lh/os/system/win/window.c` and the future paint / input
 * backends.
 *
 * Conventions: Win32 `RECT` is `{ left, top, right, bottom }` with `right`
 * and `bottom` exclusive — the opposite of `lh_rect_t::origin` + `size`.
 * `lh_rect_to_win32` writes a Win32-compatible `RECT`; `lh_rect_from_win32`
 * reads one. Same for the bit-pattern `COLORREF` (`0x00BBGGRR`), which
 * ignores alpha; `lh_color_to_win32` returns a `COLORREF` with alpha 0
 * (Win32 uses no alpha in `COLORREF`).
 */

#ifndef LH_SRC_OS_SYSTEM_WIN_GEOM_H
#define LH_SRC_OS_SYSTEM_WIN_GEOM_H

#include <lh/cast/static.h>
#include <lh/color.h>
#include <lh/geom.h>
#include <lh/os/system/win/user32.h>

/**
 * @brief Convert `lh_rect_t` (origin + size) to Win32's `RECT`
 *        (left, top, right, bottom — `right`/`bottom` exclusive).
 */
void
lh_os_system_win_rect_from_lh(const lh_rect_t *self, lh_os_system_win_rect_t *out);

/**
 * @brief Convert Win32 `RECT` (left, top, right, bottom — exclusive) to
 *        `lh_rect_t` (origin + size).
 */
lh_rect_t
lh_os_system_win_rect_to_lh(const lh_os_system_win_rect_t *self);

/**
 * @brief Convert `lh_color_t` to Win32's `COLORREF` (`0x00BBGGRR`).
 *        Alpha is dropped — `COLORREF` has no alpha channel.
 */
lh_os_system_win_colorref_t
lh_os_system_win_color_to_lh(lh_color_t self);

#endif /* LH_SRC_OS_SYSTEM_WIN_GEOM_H */