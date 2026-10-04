/**
 * @file monitor.h
 * @brief Sizes of the displays a window can sit on.
 *
 * Separate from the window and from drawing. A caller asks how big a
 * monitor is; where pixels are painted is someone else's job. The same
 * numbers are what a maximized window is fitted to.
 *
 * On Windows each entry is one monitor, and the work size is the part
 * a maximized window covers (the taskbar stays out of it). On X11 each
 * entry is one screen of the display connection. The work size there is
 * the screen size. Cocoa reports no monitors yet.
 *
 * Requires ::LH_LIBRARY_OPTION_OS_WINDOW (itself requires ::LH_LIBRARY_OPTION_OS).
 */

#ifndef LH_OS_SYSTEM_MONITOR_H
#define LH_OS_SYSTEM_MONITOR_H

#include <lh/compiler/extern/c.h>
#include <lh/config.h>
#include <lh/numeric/types.h>

#if !LH_LIBRARY_OPTION_OS
#    error "lh/os/system/monitor.h requires LH_LIBRARY_OPTION_OS (CMake: -DLH_LIBRARY_OPTION_OS=ON)"
#endif

#if !LH_LIBRARY_OPTION_OS_WINDOW
#    error "lh/os/system/monitor.h requires LH_LIBRARY_OPTION_OS_WINDOW (CMake: -DLH_LIBRARY_OPTION_OS_WINDOW=ON)"
#endif

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief How many monitors the process can see right now.
 */
lh_int_t
lh_os_system_monitor_get_count(void);

/**
 * @brief Width of monitor @p index, in pixels. Zero when @p index is out of range.
 */
lh_int_t
lh_os_system_monitor_get_width(lh_int_t index);

/**
 * @brief Height of monitor @p index, in pixels. Zero when @p index is out of range.
 */
lh_int_t
lh_os_system_monitor_get_height(lh_int_t index);

/**
 * @brief Width of the area a maximized window covers on monitor @p index.
 *
 * Zero when @p index is out of range.
 */
lh_int_t
lh_os_system_monitor_get_work_width(lh_int_t index);

/**
 * @brief Height of the area a maximized window covers on monitor @p index.
 *
 * Zero when @p index is out of range.
 */
lh_int_t
lh_os_system_monitor_get_work_height(lh_int_t index);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_SYSTEM_MONITOR_H */
