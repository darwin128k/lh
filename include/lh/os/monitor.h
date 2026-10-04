/**
 * @file monitor.h
 * @brief Sizes of the displays a window can sit on.
 *
 * See ::lh_os_system_monitor_get_count. Drawing does not live here: a
 * monitor only answers how big it is.
 *
 * Requires ::LH_LIBRARY_OPTION_OS_WINDOW (itself requires ::LH_LIBRARY_OPTION_OS).
 */

#ifndef LH_OS_MONITOR_H
#define LH_OS_MONITOR_H

#include <lh/compiler/extern/c.h>
#include <lh/config.h>
#include <lh/numeric/types.h>

#include <lh/os/system/monitor.h>

#if !LH_LIBRARY_OPTION_OS
#    error "lh/os/monitor.h requires LH_LIBRARY_OPTION_OS (CMake: -DLH_LIBRARY_OPTION_OS=ON)"
#endif

#if !LH_LIBRARY_OPTION_OS_WINDOW
#    error "lh/os/monitor.h requires LH_LIBRARY_OPTION_OS_WINDOW (CMake: -DLH_LIBRARY_OPTION_OS_WINDOW=ON)"
#endif

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief How many monitors the process can see; see ::lh_os_system_monitor_get_count.
 */
lh_int_t
lh_os_monitor_get_count(void);

/**
 * @brief Width of monitor @p index; see ::lh_os_system_monitor_get_width.
 */
lh_int_t
lh_os_monitor_get_width(lh_int_t index);

/**
 * @brief Height of monitor @p index; see ::lh_os_system_monitor_get_height.
 */
lh_int_t
lh_os_monitor_get_height(lh_int_t index);

/**
 * @brief Work width of monitor @p index; see ::lh_os_system_monitor_get_work_width.
 */
lh_int_t
lh_os_monitor_get_work_width(lh_int_t index);

/**
 * @brief Work height of monitor @p index; see ::lh_os_system_monitor_get_work_height.
 */
lh_int_t
lh_os_monitor_get_work_height(lh_int_t index);

/**
 * @brief Left edge of monitor @p index; see ::lh_os_system_monitor_get_x.
 */
lh_int_t
lh_os_monitor_get_x(lh_int_t index);

/**
 * @brief Top edge of monitor @p index; see ::lh_os_system_monitor_get_y.
 */
lh_int_t
lh_os_monitor_get_y(lh_int_t index);

/**
 * @brief Left edge of the work area; see ::lh_os_system_monitor_get_work_x.
 */
lh_int_t
lh_os_monitor_get_work_x(lh_int_t index);

/**
 * @brief Top edge of the work area; see ::lh_os_system_monitor_get_work_y.
 */
lh_int_t
lh_os_monitor_get_work_y(lh_int_t index);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_MONITOR_H */
