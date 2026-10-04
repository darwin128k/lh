/**
 * @file dwmapi.h
 * @brief Backend-private: the part of dwmapi.dll the window backend uses.
 *
 * The DLL is opened by name through the shared-image layer, not linked, so
 * a system without it still starts. An attribute the system does not know
 * fails, and the caller goes on.
 */

#ifndef LH_SRC_OS_SYSTEM_WIN_DWMAPI_H
#define LH_SRC_OS_SYSTEM_WIN_DWMAPI_H

#include <lh/bool.h>
#include <lh/numeric/types.h>
#include <lh/os/system/win/user32.h>
#include <lh/ptr.h>

/* `DWMWA_USE_IMMERSIVE_DARK_MODE` before it was documented. A BOOL.
   Windows 10 builds before 20H1. */
#define LH_OS_SYSTEM_WIN_DWMWA_USE_IMMERSIVE_DARK_MODE_BEFORE_20H1 19

/* `DWMWA_USE_IMMERSIVE_DARK_MODE`. A BOOL. Present since Windows 10 20H1.
   The process must declare Windows 10 in its manifest or this is ignored. */
#define LH_OS_SYSTEM_WIN_DWMWA_USE_IMMERSIVE_DARK_MODE 20

/* `DWMWA_BORDER_COLOR`, `DWMWA_CAPTION_COLOR`, `DWMWA_TEXT_COLOR`.
   Each a COLORREF (`0x00BBGGRR`). Windows 11. */
#define LH_OS_SYSTEM_WIN_DWMWA_BORDER_COLOR 34
#define LH_OS_SYSTEM_WIN_DWMWA_CAPTION_COLOR 35
#define LH_OS_SYSTEM_WIN_DWMWA_TEXT_COLOR 36

/* `HRESULT DwmSetWindowAttribute(HWND, DWORD, LPCVOID, DWORD)`. */
typedef lh_long_t(LH_OS_SYSTEM_WIN_CALL *lh_os_system_win_dwm_set_window_attribute_fn)(
    lh_os_system_win_hwnd_t hwnd, lh_os_system_win_dword_t attribute, const lh_ptr value,
    lh_os_system_win_dword_t size);

/**
 * @brief `DwmSetWindowAttribute` on @p window.
 *
 * @return True when the system accepted the attribute.
 */
lh_bool_t
lh_os_system_win_dwm_set_attribute(lh_os_system_win_hwnd_t window,
                                   lh_os_system_win_dword_t attribute, const lh_ptr value,
                                   lh_os_system_win_dword_t size);

/**
 * @brief `COLORREF` (`0x00BBGGRR`) from a packed `0x00RRGGBB` value.
 */
lh_os_system_win_dword_t
lh_os_system_win_colorref(lh_uint_t rgb);

#endif /* LH_SRC_OS_SYSTEM_WIN_DWMAPI_H */
