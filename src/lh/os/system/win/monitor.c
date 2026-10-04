/**
 * @file monitor.c
 * @brief Win32 monitors. `EnumDisplayMonitors` is present since Windows 2000,
 *        so Windows XP has it. dwmapi is not involved.
 */

#include <lh/bool.h>
#include <lh/cast/reinterpret.h>
#include <lh/cast/static.h>
#include <lh/null.h>
#include <lh/numeric/types.h>
#include <lh/os/system/monitor.h>
#include <lh/os/system/win/user32.h>
#include <lh/util/addr.h>

#define LH_OS_SYSTEM_WIN_MONITOR_LIMIT 16

struct lh_os_system_win_monitor_set
{
    lh_int_t count;
    lh_int_t primary;
    lh_os_system_win_rect_t bounds[LH_OS_SYSTEM_WIN_MONITOR_LIMIT];
    lh_os_system_win_rect_t work[LH_OS_SYSTEM_WIN_MONITOR_LIMIT];
};

lh_int_t LH_OS_SYSTEM_WIN_CALL
lh_os_system_win_monitor_visit(lh_os_system_win_handle_t monitor, lh_os_system_win_hdc_t dc,
                               lh_os_system_win_rect_t *bounds, lh_os_system_win_lparam_t param)
{
    struct lh_os_system_win_monitor_set *set =
        lh_cast_reinterpret(struct lh_os_system_win_monitor_set *, param);
    lh_os_system_win_monitorinfo_t info;

    (void)dc;
    if (lh_null_eq(set) || set->count >= LH_OS_SYSTEM_WIN_MONITOR_LIMIT)
    {
        return 0;
    }
    info.size = lh_cast_static(lh_os_system_win_dword_t, sizeof info);
    if (GetMonitorInfoW(monitor, lh_addr_of(info)) != 0)
    {
        set->bounds[set->count] = info.monitor;
        set->work[set->count] = info.work;
        if ((info.flags & 1U) != 0U)
        {
            set->primary = set->count;
        }
    }
    else if (!lh_null_eq(bounds))
    {
        set->bounds[set->count] = *bounds;
        set->work[set->count] = *bounds;
    }
    else
    {
        return 1;
    }
    set->count += 1;
    return 1;
}

void
lh_os_system_win_monitor_collect(struct lh_os_system_win_monitor_set *set)
{
    lh_os_system_win_rect_t swap;
    set->count = 0;
    set->primary = 0;
    (void)EnumDisplayMonitors(lh_null, lh_null, lh_os_system_win_monitor_visit,
                              lh_cast_static(lh_os_system_win_lparam_t, (lh_ptr)set));
    if (set->primary <= 0 || set->primary >= set->count)
    {
        return;
    }
    swap = set->bounds[0];
    set->bounds[0] = set->bounds[set->primary];
    set->bounds[set->primary] = swap;
    swap = set->work[0];
    set->work[0] = set->work[set->primary];
    set->work[set->primary] = swap;
}

lh_int_t
lh_os_system_monitor_edge(lh_int_t index, lh_bool_t work, lh_bool_t horizontal)
{
    struct lh_os_system_win_monitor_set set;
    const lh_os_system_win_rect_t *rect;

    lh_os_system_win_monitor_collect(lh_addr_of(set));
    if (index < 0 || index >= set.count)
    {
        return 0;
    }
    rect = work != lh_bool_false ? lh_addr_of(set.work[index]) : lh_addr_of(set.bounds[index]);
    if (horizontal != lh_bool_false)
    {
        return rect->right - rect->left;
    }
    return rect->bottom - rect->top;
}

lh_int_t
lh_os_system_monitor_get_count(void)
{
    struct lh_os_system_win_monitor_set set;

    lh_os_system_win_monitor_collect(lh_addr_of(set));
    return set.count;
}

lh_int_t
lh_os_system_monitor_get_width(lh_int_t index)
{
    return lh_os_system_monitor_edge(index, lh_bool_false, lh_bool_true);
}

lh_int_t
lh_os_system_monitor_get_height(lh_int_t index)
{
    return lh_os_system_monitor_edge(index, lh_bool_false, lh_bool_false);
}

lh_int_t
lh_os_system_monitor_get_work_width(lh_int_t index)
{
    return lh_os_system_monitor_edge(index, lh_bool_true, lh_bool_true);
}

lh_int_t
lh_os_system_monitor_get_work_height(lh_int_t index)
{
    return lh_os_system_monitor_edge(index, lh_bool_true, lh_bool_false);
}

lh_int_t
lh_os_system_monitor_corner(lh_int_t index, lh_bool_t work, lh_bool_t horizontal)
{
    struct lh_os_system_win_monitor_set set;
    const lh_os_system_win_rect_t *rect;

    lh_os_system_win_monitor_collect(lh_addr_of(set));
    if (index < 0 || index >= set.count)
    {
        return 0;
    }
    rect = work != lh_bool_false ? lh_addr_of(set.work[index]) : lh_addr_of(set.bounds[index]);
    if (horizontal != lh_bool_false)
    {
        return rect->left;
    }
    return rect->top;
}

lh_int_t
lh_os_system_monitor_get_x(lh_int_t index)
{
    return lh_os_system_monitor_corner(index, lh_bool_false, lh_bool_true);
}

lh_int_t
lh_os_system_monitor_get_y(lh_int_t index)
{
    return lh_os_system_monitor_corner(index, lh_bool_false, lh_bool_false);
}

lh_int_t
lh_os_system_monitor_get_work_x(lh_int_t index)
{
    return lh_os_system_monitor_corner(index, lh_bool_true, lh_bool_true);
}

lh_int_t
lh_os_system_monitor_get_work_y(lh_int_t index)
{
    return lh_os_system_monitor_corner(index, lh_bool_true, lh_bool_false);
}
